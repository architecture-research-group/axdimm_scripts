
# create_project -force -name comp_output
open_project /home/a972m888/Project/Kansas_1.xpr
open_hw_manager

update_compile_order -fileset sources_1
open_hw_manager
connect_hw_server -allow_non_jtag

open_hw_target

set_property PROGRAM.FILE {/home/a972m888/Project/Kansas_1.runs/impl_1/axdimm_system_top.bit} [get_hw_devices xczu19_0]
set_property PROBES.FILE {/home/a972m888/Project/Kansas_1.runs/impl_1/axdimm_system_top.ltx} [get_hw_devices xczu19_0]
set_property FULL_PROBES.FILE {/home/a972m888/Project/Kansas_1.runs/impl_1/axdimm_system_top.ltx} [get_hw_devices xczu19_0]
current_hw_device [get_hw_devices xczu19_0]
refresh_hw_device [lindex [get_hw_devices xczu19_0] 0]

display_hw_ila_data [ get_hw_ila_data hw_ila_data_1 -of_objects [get_hw_ilas -of_objects [get_hw_devices xczu19_0] -filter {CELL_NAME=~"u_ila"}]]
current_hw_device [get_hw_devices arm_dap_1]
refresh_hw_device -update_hw_probes false [lindex [get_hw_devices arm_dap_1] 0]
current_hw_device [get_hw_devices xczu19_0]

for {set i 0} {$i < 1} {incr i} {
	run_hw_ila hw_ila_1
	wait_on_hw_ila hw_ila_1
	upload_hw_ila_data hw_ila_1
	write_hw_ila_data -force -csv_file /home/a972m888/HPCA_Rebuttal/ilaData/output.csv hw_ila_data_1
}
