#!/bin/bash
OFFSET=$(( 2 ** 12 ))

R_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $R_PATH


OFFSETS=( $(( OFFSET * 2))  )
for i in "${OFFSETS[@]}"; do
		taskset -c 1 sudo ./microbench $(( i ))
done

