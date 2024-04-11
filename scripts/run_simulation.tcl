proc run_simulation {latency} {
    catch close_sim
    set_property -name {xsim.simulate.runtime} -value {10000ns} -objects [get_filesets sim_1]
    set_property -name {xsim.elaborate.xelab.more_options} -value "-generic_top \"LATENCY=$latency\"" -objects [get_filesets sim_1]
    if {[catch launch_simulation]} {
        return 1
    }
    set simErr [get_value -radix unsigned err]
    close_sim
    if {$simErr eq 1} {
        return 2
    }
    return 0
}

# lassign $args tb ref dut latency
# run_simulation $tb $ref $dut $latency

#set latency 16
#set err [run_simulation $latency]
# set_property top $tb [get_filesets sim_1]
#set_property -name {xsim.elaborate.xelab.more_options} -value "-generic_top \"LATENCY=$latency\"" -objects [get_filesets sim_1]
# set launchErr [launch_simulation]
# puts $launchErr
# if {[catch launch_simulation]} {
#     set err 1
# } else {
#     set simErr [get_value -radix unsigned err]
#     if {$simErr eq 1} {
#         set err 2
#     } else {
#         set err 0
#     }
# }
# set simErr [get_value -radix unsigned /HLSM_tb(LATENCY=$latency)/err]
# puts $simErr