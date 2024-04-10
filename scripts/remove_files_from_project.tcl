# Get project filesets
set filesets [get_filesets]

puts $filesets

# Loop through each of the project filesets
foreach fileset $filesets {
    
    # Get files in the fileset
    set prj_files [get_files -of_objects [get_filesets $fileset] -quiet]
    
    # Remove all files from the project
    if {[llength $prj_files]} {
        foreach prj_file $prj_files {
            remove_files -fileset $fileset $prj_file
        }
    }
}