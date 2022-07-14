#!/bin/bash

high_bits=( "28" )

copyChar=69

[ ! -z "$( lsmod | grep rw_test)" ] && ./uninstall.sh

for i in "${high_bits[@]}"; do
	o1=$( echo "$i" | grep  -Eo "[0-9]+" | head -n 1 )
	o2=$( echo "$i" | grep  -Eo "[0-9]+" | tail -n 1 )

	#./install.sh test=1
	./install.sh test=0 uchar=$copyChar offset=$(( 2 ** $o1 )) offset2=$(( 2 ** $o2 )) 
	read -n 1 -p "exported ${i}.csv to /tmp ?" foo
	csv_n=${i}.csv
	scp castor:/tmp/$csv_n .
	awk -F, '$12~/69/{printf("Command #:%s ddr:%s data:%s\n", NR-1 , $8, $12);}'  $csv_n | grep 65
	./uninstall.sh
done

for i in "${triple_bits[@]}"; do
	o1=$( echo "$i" | grep  -Eo "[0-9]+" | head -n 1 )
	o2=$( echo "$i" | grep  -Eo "[0-9]+" | head -n 2 | tail -n 1 )
	o3=$( echo "$i" | grep  -Eo "[0-9]+" | tail -n 1)

	./install.sh test=0 uchar=$copyChar offset=$(( 2 ** $o1 )) offset2=$(( 2 ** $o2 )) offset3=$(( 2 ** $o3 ))
	read -n 1 -p "exported ${i}.csv to /tmp ?" foo
	csv_n=${i}.csv
	scp castor:/tmp/$csv_n .
	awk -F, '$12~/65/{printf("Command #:%s ddr:%s data:%s\n", NR-1 , $8, $12);}'  $csv_n | grep 65
	./uninstall.sh
done

for i in *.csv; do
	read -n 1 -p "exported ${i}.csv to /tmp ?" foo
	echo -n "Function: $(echo "$i" | sed 's/\.csv//g'): "
	awk -F, '$12~/61/{printf("Command #:%s ddr:%s data:%s\n", NR-1 , $8, $12);}' $i | grep 61
done
exit

