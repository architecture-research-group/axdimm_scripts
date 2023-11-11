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


##### Address decoding and src/dst buffer validation
```sh
cd /home/a972m888/HPCA_CameraReady_Data/Emulator/vivadoScript
# should contain: ILA.tcl  logInAndArm.sh  ResetAxdimm.tcl  vivado.jou  vivado.log -- backups of tcl and sh scripts in ./ila-scripts
./logInAndArm.sh AddrRes trace

cp /home/n869p538/AddrRes/*.csv ./ila_data
python3 main.py
```

```
make
./run.sh
```