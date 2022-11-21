#!/bin/bash
CORES=( "1" "2" "3" "4" "5" "6" "7" "8" "9" "10")
SIZE=$(( 2 ** 26 ))

sudo pqos -R
sudo pqos -e "llc:0=0x0001"
sudo pqos -a "llc:0=0-10"

rm -f worker_log
antagonist(){
	#while [ "1" ]; do
	taskset -c 0 sudo ./antagonist 
	#done

}

antagonist &
#worker event loop
while [ "1" ]; do 
	for i in "${CORES[@]}"; do
		taskset -c ${i} sudo ./worker $(( (i * SIZE) )) $(( ( (i * SIZE) ) + ( 2 ** 28 ) )) >> worker_log &
	done
	wait
done

wait
