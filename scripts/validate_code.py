
import argparse
from colorama import Fore, Style, init
from enum import IntEnum
import psutil
import os
import pandas
import shutil
import subprocess
import sys
 
if psutil.Process(os.getpid()).parent().name() == 'cmd.exe':
    init(convert=True)

def is_windows():
    return os.name == 'nt'

def lists_are_equal(x, y):
    is_equal = False
    if len(x) == len(y):
        is_equal = all([xi == yi for (xi, yi) in zip(x, y)])
    return is_equal
    
class RunStatus(IntEnum):
    NO_ERROR        = 0
    BAD_ERROR_LEVEL = 1
    BAD_ERROR_MSG   = 2
    SEG_FAULT       = 3
    NO_COMPILE      = 4
    BAD_BEHAV       = 5
        
class validationSuite:
            
    def __init__(self, skip_vivado=False, vivado_only=False):
        self.init_path = os.getcwd()
        self.class_path = os.path.dirname(__file__)
        self.project_dir = os.path.abspath(os.path.join(self.class_path,'..'))
        self.build_dir = os.path.join(self.project_dir,'build')
        self.autogen_dir = os.path.join(self.project_dir,'verilog_files','autogen')
        self.regression_dir = os.path.join(self.project_dir,'regression')
        self.mingw_path = self.get_mingw_path()
        self.skip_vivado = skip_vivado
        self.vivado_only = vivado_only
        self.in_files = None
        self.latencies = None
        self.out_files = None
        self.error_levels = None
        self.error_messages = None
        self.test_names = None
        self.status = None
        self.regression_status = None     
        
    def get_mingw_path(self):
        if is_windows():
            cmd = 'where gcc'
            shell = False
        else:
            cmd = 'which gcc'
            shell = True
        r = subprocess.run(cmd, shell=shell, capture_output=True)
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
        if is_windows():
            r = subprocess.run('cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE ' + \
                f'-DCMAKE_C_COMPILER:FILEPATH={self.mingw_path}\\gcc.exe -DCMAKE_CXX_COMPILER:FILEPATH={self.mingw_path}\\g++.exe -S{self.project_dir} ' + \
                f'-B{self.build_dir} -G "MinGW Makefiles"')
            if r.returncode:
                self.throw_error("Failed to configure cmake")
        if is_windows():
            shell = False
        else:
            shell = True
        r = subprocess.run('cmake ..', shell=shell)
        if r.returncode:
            self.throw_error("Failed to configure cmake")
        
    def compile_code(self):
        self.remove_old_build_directory()
        os.mkdir(self.build_dir)
        os.chdir(self.build_dir)
        self.configure_cmake()
        if is_windows():
            shell = False
        else:
            shell = True
        r = subprocess.run('cmake --build .', shell=shell)
        if r.returncode:
            self.throw_error("Failed to compile code")
        os.chdir(self.init_path)
        
    def load_tests(self):
        csv_path = os.path.join(self.class_path, 'validation_tests.csv')
        df = pandas.read_csv(csv_path)
        test_names = df['Test Name']
        latencies = df['Latency']
        error_levels = df['Error Level']
        error_messages = df['Error Message']
        self.in_files = []
        self.latencies = []
        self.out_files = []
        self.error_levels = []
        self.test_names = []
        self.error_messages = []
        for (test_name, latency, error_level, error_message) in zip(test_names, latencies, error_levels, error_messages):
            in_file = f'{self.project_dir}/testfiles/{test_name}'
            test_name = os.path.splitext(os.path.basename(test_name))[0]
            out_file = f'{self.project_dir}/verilog_files/autogen/{test_name}.v'
            self.test_names.append(test_name)
            self.in_files.append(in_file)
            self.out_files.append(out_file)
            self.latencies.append(latency)
            self.error_levels.append(error_level)
            self.error_messages.append(error_message)
            
    def create_new_autogen_dir(self):
        try:
            if os.path.exists(self.autogen_dir):
                shutil.rmtree(self.autogen_dir)
            os.mkdir(self.autogen_dir)
        except:
            self.throw_error("Could not remove autogen directory")
        
    def create_new_regression_dir(self):
        try:
            if os.path.exists(self.regression_dir):
                shutil.rmtree(self.regression_dir)
            os.mkdir(self.regression_dir)
        except:
            self.throw_error("Could not remove regression directory")
    
    def validate_regression_file(self):
        regression_file = os.path.join(self.regression_dir, 'regression.csv')
        try:
            df = pandas.read_csv(regression_file)
        except:
            self.throw_error("Could not find regression directory")
        test_names = df['Test Name']
        latencies = df['Latency']
        self.regression_status = df['Error Level']
        test_names_are_equal = lists_are_equal(test_names, self.test_names)
        latencies_are_equal = lists_are_equal(latencies, self.latencies)
        if not test_names_are_equal or not latencies_are_equal:
            self.throw_error("Invalid regression file")
            
    def save_regression_output(self):
        regression_file = os.path.join(self.regression_dir, 'regression.csv')
        with open(regression_file, 'a') as f:
            f.write('Test Name,Latency,Error Level\n');
            for idx in range(len(self.test_names)):
                f.write(f'{self.test_names[idx]},{self.latencies[idx]},{int(self.status[idx])}\n')
            
    def run_tests(self):      
        self.status = []
        for idx in range(len(self.test_names)):
            self.run_test(idx)
            
    def run_test(self, idx):
        regression_dir = os.path.join(self.regression_dir,str(idx))
        if self.vivado_only:
            if self.regression_status[idx]:
                self.status.append(self.regression_status[idx])
                return
            src_file = os.path.join(regression_dir, f'{self.test_names[idx]}.v')
            dest_file = self.out_files[idx]
            if os.path.exists(src_file):
                shutil.copyfile(src_file, dest_file)
            else:
                self.status.append(self.regression_status[idx])
                return
        else:
            os.chdir(self.build_dir)
            if is_windows():
                cmd='./src/hlsyn.exe'
                shell=False
            else:
                cmd='./src/hlsyn'
                shell=True
            r = subprocess.run(f'{cmd} {self.in_files[idx]} {self.latencies[idx]} {self.out_files[idx]}', shell=shell, capture_output=True)
            if r.returncode != self.error_levels[idx]:
                self.status.append(RunStatus.BAD_ERROR_LEVEL)
                return
            elif r.returncode != 0:
                cmd_output = r.stdout.decode().rstrip()
                if (cmd_output == self.error_messages[idx]):
                    self.status.append(RunStatus.NO_ERROR)
                else:
                    self.status.append(RunStatus.BAD_ERROR_MSG)
                return
            if not os.path.exists(self.out_files[idx]):
                self.status.append(RunStatus.SEG_FAULT)
                return
            os.mkdir(regression_dir)
            src_file = self.out_files[idx]
            dest_file = os.path.join(regression_dir, f'{self.test_names[idx]}.v')
            shutil.copy(src_file, dest_file)
        if self.skip_vivado:
            self.status.append(RunStatus.NO_ERROR)
        else:
            os.chdir(self.project_dir)
            r = subprocess.run(f'vivado -mode batch -nolog -nojournal -source .\\scripts\\run_test.tcl -tclargs {self.test_names[idx]} {self.latencies[idx]}', shell=True)
            if r.returncode == 0:
                self.status.append(RunStatus.NO_ERROR)
            elif r.returncode == 1:
                self.status.append(RunStatus.NO_COMPILE)
            else:
                self.status.append(RunStatus.BAD_BEHAV)
        
    def get_status_printout(self, status):
        if status == RunStatus.NO_ERROR:
            return "Passed"
        elif status == RunStatus.BAD_ERROR_LEVEL:
            return "Error: Incorrect Error Level"
        elif status == RunStatus.BAD_ERROR_MSG:
            return "Error: Incorrect Error Message"
        elif status == RunStatus.SEG_FAULT:
            return "Error: Segmentation Fault"
        elif status == RunStatus.NO_COMPILE:
            return "Error: Verilog Compilation Failed"
        elif status == RunStatus.BAD_BEHAV:
            return "Error: Behavioral Simulation Failed"
        else:
            self.throw_error("Unrecognized run status")
            
    def print_results(self):
        print('\nTEST RESULTS:\n')
        print('%-20s | %-7s | %s' % ('Test Name', 'Latency', 'Result'))
        print('%s' % ('-' * 60))
        for idx in range(len(self.test_names)):
            test_name = self.test_names[idx] + ".c"
            status = self.get_status_printout(self.status[idx])
            if self.status[idx] == RunStatus.NO_ERROR:
                status = f'{Fore.GREEN}{status}{Style.RESET_ALL}'
            else:
                status = f'{Fore.RED}{status}{Style.RESET_ALL}'
            print(f'{test_name:20s} | {self.latencies[idx]:7d} | {status}')
        print()
        if self.skip_vivado:
            print(f'{Fore.YELLOW}Warning: No vivado validation performed. ' \
                f'Copy to machine with vivado and rerun with --vivado-only flag{Style.RESET_ALL}\n')
         
    def get_git_hash(self):
        return subprocess.check_output(['git', 'rev-parse', 'HEAD']).decode('ascii').strip()

    def is_git_repo_dirty(self):
        cmd_output = subprocess.check_output(['git', 'diff', '--stat']).decode('ascii').strip()
        if cmd_output == '':
            return False
        else:
            return True
            
    def log_git_hash(self):
        git_hash = self.get_git_hash()
        is_git_repo_dirty = self.is_git_repo_dirty()
        git_log = os.path.join(self.regression_dir,'git.log')
        with open(git_log,'w') as f:
            f.write(f'{git_hash}\n')
            if is_git_repo_dirty:
                f.write('git repo contains uncommitted changes')
        
    def check_git_hash(self):
        git_hash = self.get_git_hash()
        is_git_repo_dirty = self.is_git_repo_dirty()
        git_log = os.path.join(self.regression_dir,'git.log')
        with open(git_log, 'r') as f:
            lines = f.readlines()
            if (git_hash != lines[0]):
                print(f'{Fore.YELLOW}Warning: Mismatch in git hash ' \
                    f'between local and remote machine{Style.RESET_ALL}\n')
            elif (is_git_repo_dirty or len(lines) > 1):
                print(f'{Fore.YELLOW}Warning: Either local or remote ' \
                    f'machine contain uncommitted changes{Style.RESET_ALL}\n')
                    
    def run(self):
        self.load_tests()
        if not self.vivado_only:
            self.compile_code()
            self.create_new_autogen_dir()
            self.create_new_regression_dir()
        else:
            self.validate_regression_file()        
        self.run_tests()
        self.print_results()
        if self.vivado_only:
            self.check_git_hash()
        else:
            self.save_regression_output()
            self.log_git_hash()
        
if __name__=="__main__":

    parser = argparse.ArgumentParser(
        prog="validate_code",
        description="Function validates execution and behavior of high-level synthesis tool")
        
    parser.add_argument('-s', '--skip-vivado', action='store_true',
        help='skip vivado step of code execution')
        
    parser.add_argument('-r', '--vivado-only', action='store_true',
        help='only run vivado step of code execution')
        
    args = parser.parse_args()
    if args.skip_vivado and args.vivado_only:
        print('Error: Cannot run in both skip-vivado and vivado-only mode')
        
    validation_suite = validationSuite(
        skip_vivado=args.skip_vivado,
        vivado_only=args.vivado_only)
        
    validation_suite.run()
    sys.exit(0)