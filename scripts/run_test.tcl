proc run_test {test_name latency} {
    source ./scripts/open_project.tcl
    source ./scripts/remove_files_from_project.tcl
    source ./scripts/add_files_to_project.tcl
    source ./scripts/run_simulation.tcl
    add_files_to_project $test_name
    return [run_simulation $latency]
}

if {$argc eq 2} {
    set test_name [lindex $argv 0]
    set latency [lindex $argv 1]
    exit [run_test $test_name $latency]
}