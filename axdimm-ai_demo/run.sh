dmesg -C
dmesg -D
make 
dmesg -E
insmod rw_test.ko $*
dmesg -D
make clean
dmesg -E
rmmod rw_test 
dmesg | tee output.txt
