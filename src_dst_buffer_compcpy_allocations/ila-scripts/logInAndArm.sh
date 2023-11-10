#!/bin/bash

# Delete all the old/temporary files
rm *jou
rm *log
rm -r .Xil

# arguments for the script are:
# $1: the name of the folder for collecting the traces
# $2: the name of the files
# $4: the number of antagonists

# Define the remote machine's SSH details
remote_host="axdimm.ittc.ku.edu"
remote_user="a972m888"
remote_directory="/home/a972m888/hpca/axdimm_scripts/address_resolution/"
remote_script="run.sh"

if [ "$3" = "reset" ]; then
    # Define the command to run locally
    local_command="vivado -mode batch -source ResetAxdimm.tcl"

    # Run the local command
    $local_command 
else
    # Define the command to run locally
    local_command="vivado -mode batch -source ILA.tcl -tclargs $1 $2 1"

    # Run the local command
    $local_command

    # Wait for the FPGA to be ptogrammed and armed with the ILA
    sleep 40

    # SSH into the remote machine, change to the specified directory, and run the script
    # ssh $remote_user@$remote_host "cd $remote_directory && timeout 30 ./$remote_script"

fi  
