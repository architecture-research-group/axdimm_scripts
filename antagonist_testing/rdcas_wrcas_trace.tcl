create_project -force -name comp_output
run_hw_ila hw_ila_1
wait_on_hw_ila hw_ila_1
upload_hw_ila_data hw_ila_1
write_hw_ila_data -csv_file /home/a972m888/HPCA_Rebuttal/ilaData/1workers-1-632-1way_$i.csv hw_ila_data_1a
