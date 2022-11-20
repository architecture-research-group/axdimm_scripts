#!/bin/bash
CORES=( "1" "2" "3" "4" "5" "6" "7" "8" "9" "10")

sudo pqos -R
sudo pqos -e "llc:0=0x0001"
sudo pqos -a "llc:0=0-10"

antagonist(){
	#while [ "1" ]; do
	taskset -c 0 sudo ./antagonist
	#done

}

antagonist &
#worker event loop
while [ "1" ]; do 
	for i in "${CORES[@]}"; do
		taskset -c ${i} sudo ./worker $(( (i * 4096) + 2 ** 22 )) $(( ( (i * 4096) + (2 ** 22) ) + ( 2** 22 ) )) &
	done
	wait
done
