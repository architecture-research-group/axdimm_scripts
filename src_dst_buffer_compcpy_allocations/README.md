#### Allocate Src and Dst buffers for Simple Reverse Translation from Dst to Src

***Goal***: choose pairs of Src and Dst Buffers whose physical DRAM mappings are equivalent aside from 1-2 bits
(1) Different Bank Address Same (BG, ..., Row, and Column)
* Choosing Src and Dst buffers with matching physical DRAM mappings aside from the ***BA*** bits:
(0x100000000, 0x100081200) -- how far does this matching mapping extend (i.e., all bits match with BG toggled)

tabulated pairs (xcel rows): (4307,8403) (4223, 8319), ..., 

##### Implementation
* Program Copies from a src buffer to a dst buffer whose addresses differ only in the BA field

* restrictions on source buffers:
    * source buffers must have a BA of 00

* restrictions on dst buffers:
    * must have a BA of 01

```
make
./run.sh
```

Output:
* TODO: report DRAM addresses and data for each RdCAS and WrCAS corresponding to the "_off" addresses below
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