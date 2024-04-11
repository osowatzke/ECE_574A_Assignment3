proc add_files_to_project {test_name} {
    add_files -fileset sources_1 ./verilog_files/autogen/${test_name}.v
    add_files -fileset sim_1     ./verilog_files/reference/${test_name}_ref.v
    add_files -fileset sim_1     ./verilog_files/tb/${test_name}_tb.v
    add_files -fileset sim_1     ./verilog_files/tb/clk_gen.v
    add_files -fileset sim_1     ./verilog_files/tb/rst_gen.v
    add_files -fileset sim_1     ./verilog_files/tb/delay_gen.v
    add_files -fileset sim_1     ./verilog_files/tb/error_monitor.v
}