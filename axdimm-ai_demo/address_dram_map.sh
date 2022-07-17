#!/bin/bash

source common.sh

[ ! -z "$( lsmod | grep rw_test)" ] && ./uninstall.sh

max=$(( 1024 * 32 ))
#max=$(( 64 * 3 ))

./uninstall.sh
ctr=1
for i in `seq 0 64 $max` ; do
	echo "$i, $( offset_to_addr $i)"
	./install.sh test=1 offset=$i
	./uninstall.sh
	echo "iteration: $ctr complete"
	ctr=$(( ctr + 1 ))
done

exit
