#!/bin/bash
CORES=( "1" "2" "3" "4" "5" "6" "7" "8" )
CORES=( "1" )
SIZE=${1}
WAYMASK=${2}

R_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $R_PATH

sudo pqos -R
sudo pqos -e "llc:0=$WAYMASK"
sudo pqos -a "llc:0=0-10"

sed "s/#define SIZE .*/#define SIZE $SIZE/g" microbench_template.c > microbench.c
rm -f microbench
make microbench

DST_BUFFERS_OFFSET=$(( SIZE * 9 ))
SRC_BUFFERS_OFFSET=$(( SIZE ))
rm -f worker_log
while [ $iter -lt 3 ]; do 
	for i in "${CORES[@]}"; do
		echo "taskset -c ${i} sudo ./microbench $(( SRC_BUFFERS_OFFSET + ( SIZE * i ) )) $(( DST_BUFFERS_OFFSET + (SIZE * i ) )) | tee -a worker_log &"
		iter=$(( iter + 1 ))
		sleep 2
	done
	wait
done

wait
