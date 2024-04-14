import os
import pandas
import sys
import subprocess
import shutil
 
class validationSuite:
    def __init__(self):
        self.init_path = os.getcwd()
        self.class_path = os.path.dirname(__file__)
        self.project_dir = os.path.abspath(os.path.join(self.class_path,'..'))
        self.build_dir = os.path.join(self.project_dir,'build')
        self.autogen_dir = os.path.join(self.project_dir,'verilog_files','autogen')
        self.mingw_path = self.get_mingw_path()
        self.in_files = None
        self.latencies = None
        self.out_files = None
        self.error_levels = None
        self.test_names = None
        self.status = None
        
        
    def get_mingw_path(self):
        r = subprocess.run('where gcc', capture_output=True)
        if r.returncode:
            self.throw_error("Could not find gcc installation")
        gcc_path = r.stdout.decode().rstrip()
        mingw_path = os.path.dirname(gcc_path)
        return mingw_path
    
    def throw_error(self, msg):
        os.chdir(self.init_path)
        print(msg)
        sys.exit(1)
        
    def remove_old_build_directory(self):
        try:
            if os.path.exists(self.build_dir):
                shutil.rmtree(self.build_dir)
        except:
            self.throw_error("Could not remove build directory")
            
    def configure_cmake(self):
        r = subprocess.run('cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE ' + \
            f'-DCMAKE_C_COMPILER:FILEPATH={self.mingw_path}\\gcc.exe -DCMAKE_CXX_COMPILER:FILEPATH={self.mingw_path}\\g++.exe -S{self.project_dir} ' + \
            f'-B{self.build_dir} -G "MinGW Makefiles"')
        if r.returncode:
            self.throw_error("Failed to configure cmake")
        r = subprocess.run('cmake ..')
        if r.returncode:
            self.throw_error("Failed to configure cmake")
        
    def compile_code(self):
        self.remove_old_build_directory()
        os.mkdir(self.build_dir)
        os.chdir(self.build_dir)
        self.configure_cmake()
        r = subprocess.run('cmake --build .')
        if r.returncode:
            self.throw_error("Failed to compile code")
        os.chdir(self.init_path)
        
    def load_tests(self):
        csv_path = os.path.join(self.class_path, 'validation_tests.csv')
        df = pandas.read_csv(csv_path)
        test_names = df['Test Name']
        latencies = df['Latency']
        error_levels = df['Error Level']
        self.in_files = []
        self.latencies = []
        self.out_files = []
        self.error_levels = []
        self.test_names = []
        for (test_name, latency, error_level) in zip(test_names, latencies, error_levels):
            in_file = f'{self.project_dir}/testfiles/{test_name}'
            test_name = os.path.splitext(os.path.basename(test_name))[0]
            out_file = f'{self.project_dir}/verilog_files/autogen/{test_name}.v'
            self.test_names.append(test_name)
            self.in_files.append(in_file)
            self.out_files.append(out_file)
            self.latencies.append(latency)
            self.error_levels.append(error_level)
            
    def create_new_autogen_dir(self):
        try:
            if os.path.exists(self.autogen_dir):
                shutil.rmtree(self.autogen_dir)
            os.mkdir(self.autogen_dir)
        except:
            self.throw_error("Could not remove autogen directory")
        
    def run_tests(self):
        self.status = []
        for idx in range(len(self.test_names)):
            self.run_test(idx)
            
    def run_test(self, idx):
        os.chdir(self.build_dir)
        r = subprocess.run(f'./src/hlsyn.exe {self.in_files[idx]} {self.latencies[idx]} {self.out_files[idx]}')
        if r.returncode != self.error_levels[idx]:
            self.status.append("Error: Incorrect ErrorLevel")
            return
        elif r.returncode != 0:
            self.status.append("Passed")
            return
        if not os.path.exists(self.out_files[idx]):
            self.status.append("Error: Segmentation Fault")
            return
        os.chdir(self.project_dir)
        print(os.getcwd())
        print(f'vivado -mode batch -nolog -nojournal -source .\\scripts\\run_test.tcl -tclargs {self.test_names[idx]} {self.latencies[idx]}')
        r = subprocess.run(f'vivado -mode batch -nolog -nojournal -source .\\scripts\\run_test.tcl -tclargs {self.test_names[idx]} {self.latencies[idx]}', shell=True)
        if r.returncode == 0:
            self.status.append("Passed")
        elif r.returncode == 1:
            self.status.append("Error: Verilog Compilation Failed")
        else:
            self.status.append("Error: Behavioral Simulation Failed")
            
    def print_results(self):
        print('\nTEST RESULTS:')
        for idx in range(len(self.test_names)):
            test_name = self.test_names[idx] + ".c"
            print(f'{test_name:20s}: {self.status[idx]}')
        print()
        
    def run(self):
        self.compile_code()
        self.create_new_autogen_dir()
        self.load_tests()
        self.run_tests()
        self.print_results()
        
if __name__=="__main__":
    validation_suite = validationSuite()
    validation_suite.run()
    sys.exit(0)