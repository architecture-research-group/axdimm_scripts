#!/bin/bash
CORES=( "1" "2" "3" "4" "5" "6" "7" "8" "9" "10")
SIZE=$(( 2 ** 27 ))

sudo pqos -R
sudo pqos -e "llc:0=0x000F"
sudo pqos -a "llc:0=0-10"

rm -f worker_log
#worker event loop
while [ "1" ]; do 
	for i in "${CORES[@]}"; do
		taskset -c ${i} sudo ./worker $(( (i * SIZE) )) $(( ( (i * SIZE) ) + ( 2 ** 28 ) )) >> worker_log &

	done
	wait
done

wait
