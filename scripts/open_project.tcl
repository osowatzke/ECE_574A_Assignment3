if {[file exists project]} {
    if {[catch {current_project} result]} {
        open_project ./project/project.xpr
    }
} else {
    source ./scripts/project.tcl
}