open_hw_manager
connect_hw_server -allow_non_jtag
open_hw_target
current_hw_device [get_hw_devices xczu19_0]
refresh_hw_device -update_hw_probes false [lindex [get_hw_devices xczu19_0] 0]
current_hw_device [get_hw_devices arm_dap_1]
refresh_hw_device -update_hw_probes false [lindex [get_hw_devices arm_dap_1] 0]
current_hw_device [get_hw_devices xczu19_0]



# set the bitstream and ltx file  -- NOTE: change the path to the bitstream and ltx file
set_property PROGRAM.FILE {/home/a972m888/OldDirectories/AXDIMM_example.bit} [get_hw_devices xczu19_0]

# program the device
program_hw_devices [get_hw_devices xczu19_0]
refresh_hw_device [lindex [get_hw_devices xczu19_0] 0]
