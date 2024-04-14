proc run_simulation {latency} {
    catch close_sim
    set_property top HLSM_tb [get_fileset sim_1]
    set_property -name {xsim.simulate.runtime} -value {10000ns} -objects [get_filesets sim_1]
    set_property -name {xsim.elaborate.xelab.more_options} -value "-generic_top \"LATENCY=$latency\"" -objects [get_filesets sim_1]
    if {[catch launch_simulation]} {
        return 1
    }
    set simErr [get_value -radix unsigned err]
    if {$simErr eq 1} {
        return 2
    }
    return 0
}