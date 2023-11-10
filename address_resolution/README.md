#### Extract DRAM mappings

```
make
./run.sh
# check ILAs for received addresses and data
```

Output:
* TODO: report DRAM addresses for each "*off" address below 
```
taskset -c 1 sudo ./microbench 0 16384
using src_off: 0x0 -- TODO
using dst_off: 0x4000 -- TODO
taskset -c 1 sudo ./microbench 4096 20480
using src_off: 0x1000
using dst_off: 0x5000
taskset -c 1 sudo ./microbench 8192 24576
using src_off: 0x2000
using dst_off: 0x6000
taskset -c 1 sudo ./microbench 12288 28672
using src_off: 0x3000
using dst_off: 0x7000
```