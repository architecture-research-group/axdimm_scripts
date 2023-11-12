#!/bin/bash

R_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $R_PATH

make
echo "taskset -c 1 sudo ./microbench"
taskset -c 1 sudo ./microbench