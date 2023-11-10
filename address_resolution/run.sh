#!/bin/bash
SIZE=$(( 2 ** 12 ))

R_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $R_PATH


SRC_BUFFERS_OFFSETS=( 0 $SIZE $(( SIZE * 2)) $(( SIZE * 3)) )
DST_BUFFERS_OFFSET=( $((SIZE * 4 )) $((SIZE * 5 )) $((SIZE * 6 )) $(( SIZE * 7 )) )
j=0
for i in "${SRC_BUFFERS_OFFSETS[@]}"; do
		echo "taskset -c 1 sudo ./microbench $(( i )) $(( DST_BUFFERS_OFFSET[j] ))"
		taskset -c 1 sudo ./microbench $(( i )) $(( DST_BUFFERS_OFFSET[j] ))
		j=$(( j + 1 ))
done

