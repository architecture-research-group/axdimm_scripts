#!/bin/bash
CORES=( "1" "2" "3" "4" "5" "6" "7" "8" )
SIZE=$(( 2 ** 21 ))
WAYMASK=0x0001

sudo pqos -R
sudo pqos -e "llc:0=$WAYMASK"
sudo pqos -a "llc:0=0-10"

sed "s/#define SIZE .*/#define SIZE $SIZE/g" microbench_template.c > microbench.c
rm -f microbench
make microbench

DST_BUFFERS_OFFSET=$(( SIZE * 9 ))
SRC_BUFFERS_OFFSET=$(( SIZE ))
rm -f worker_log
while [ "1" ]; do 
	for i in "${CORES[@]}"; do
		#echo "taskset -c ${i} sudo ./worker $(( (i * SIZE) )) $(( ( (i * SIZE) ) + ( SIZE ) )) >> worker_log &"
		taskset -c ${i} sudo ./microbench $(( SRC_BUFFERS_OFFSET + ( SIZE * i ) )) $(( DST_BUFFERS_OFFSET + (SIZE * i ) )) | tee -a worker_log &
	done
	wait
done

wait
