# This script is used to collect traces from the FPGA 


# Check the number of command-line arguments
if { $argc != 3 } {
        puts "The script requires three names to be input."
        puts "Usage: vivado -mode batch -source ila.tcl -tclargs <folderName> <fileName> <setup 0: collecting traces 1: program the FPGA and setup the testbed >"
    } else {
        set folderName [lindex $argv 0]
        set fileName [lindex $argv 1]
        set setup [lindex $argv 2]
    }

if {$setup == 1} {
    # open the hardware manager and connect to the board
    open_hw_manager
    connect_hw_server -allow_non_jtag
    open_hw_target
    current_hw_device [get_hw_devices xczu19_0]
    refresh_hw_device -update_hw_probes false [lindex [get_hw_devices xczu19_0] 0]
    current_hw_device [get_hw_devices arm_dap_1]
    refresh_hw_device -update_hw_probes false [lindex [get_hw_devices arm_dap_1] 0]
    current_hw_device [get_hw_devices xczu19_0]



    # set the bitstream and ltx file  -- NOTE: change the path to the bitstream and ltx file
    set_property PROBES.FILE {/home/a972m888/HPCA_CameraReady_Data/Emulator/vivadoProject/impl_1/axdimm_system_top.ltx} [get_hw_devices xczu19_0]
    set_property FULL_PROBES.FILE {/home/a972m888/HPCA_CameraReady_Data/Emulator/vivadoProject/impl_1/axdimm_system_top.ltx} [get_hw_devices xczu19_0]
    set_property PROGRAM.FILE {/home/a972m888/HPCA_CameraReady_Data/Emulator/vivadoProject/impl_1/axdimm_system_top.bit} [get_hw_devices xczu19_0]

    # program the device
    program_hw_devices [get_hw_devices xczu19_0]
    refresh_hw_device [lindex [get_hw_devices xczu19_0] 0]
    set_property CONTROL.TRIGGER_POSITION 100 [get_hw_ilas -of_objects [get_hw_devices xczu19_0] -filter {CELL_NAME=~"u_ila"}]
    display_hw_ila_data [ get_hw_ila_data hw_ila_data_1 -of_objects [get_hw_ilas -of_objects [get_hw_devices xczu19_0] -filter {CELL_NAME=~"u_ila"}]]
    # set the trigger to be the rising edge of the RdCAS signal
    set_property CONTROL.TRIGGER_CONDITION OR [get_hw_ilas -of_objects [get_hw_devices xczu19_0] -filter {CELL_NAME=~"u_ila"}]
    set_property TRIGGER_COMPARE_VALUE eq1'b1 [get_hw_probes memPHY_0_WrCAS -of_objects [get_hw_ilas -of_objects [get_hw_devices xczu19_0] -filter {CELL_NAME=~"u_ila"}]]
    set_property TRIGGER_COMPARE_VALUE eq1'b1 [get_hw_probes memPHY_0_RdCAS -of_objects [get_hw_ilas -of_objects [get_hw_devices xczu19_0] -filter {CELL_NAME=~"u_ila"}]]
}


# set folderName [lindex $argv 0]
file mkdir "/home/n869p538/$folderName"
# Define the folder path where you want to save the files
set folder_path "/home/n869p538/$folderName/"

set i 0  ;# Initialize the loop variable
# Loop to create and save 5 CSV files
for {set i 0} {$i < 1000} {incr i} {
    # set file_name "${folder_path}$fileName${i}.csv"
    run_hw_ila hw_ila_1
    wait_on_hw_ila hw_ila_1
    upload_hw_ila_data hw_ila_1
    write_hw_ila_data -csv_file $folder_path/$fileName-$i.csv hw_ila_data_1 -force
    # puts "Saved data to $file_name"
}

