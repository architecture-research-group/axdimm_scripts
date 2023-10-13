#!/bin/bash
CORES=( "1" "2" "3" "4" "5" "6" "7" "8" )
SIZE=$(( 2 ** 21 ))

sudo pqos -R
#sudo pqos -e "llc:0=0x0001"
#sudo pqos -e "llc:0=0x0003"
#sudo pqos -e "llc:0=0x000F"
#sudo pqos -e "llc:0=0x00FF"
sudo pqos -e "llc:0=0x07FF"
sudo pqos -a "llc:0=0-10"

rm -f microbench
make microbench

taskset -c 1 sudo ./microbench $(( (SIZE) )) $((   SIZE  )) | tee -a worker_log &

wait
