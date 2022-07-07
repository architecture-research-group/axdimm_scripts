#!/bin/bash

single_bits=( "6" "7" "9" "13" "15" "20" )
double_bits=( "8_10" "8_11" "12_14" "12_16" "12_17" "18_19" "12_21" "18_22" "18_23" "18_24" "8_25" "18_26" "8_27" "8_28" "8_29" "12_30" )
triple_bits=( "8_12_18" )

[ ! -z "$( lsmod | grep rw_test)" ] && ./uninstall.sh

for i in "${double_bits[@]}"; do
	o1=$( echo "$i" | grep  -Eo "[0-9]+" | head -n 1 )
	o2=$( echo "$i" | grep  -Eo "[0-9]+" | tail -n 1 )

	./install.sh test=0 uchar=65 offset=$(( 2 ** $o1 )) offset2=$(( 2 ** $o2 )) 
	read -n 1 -p "exported ${i}.csv to /tmp ?" foo
	csv_n=${i}.csv
	scp castor:/tmp/$csv_n .
	awk -F, '$12~/65/{printf("Command #:%s ddr:%s data:%s\n", NR-1 , $8, $12);}'  $csv_n | grep 65
	./uninstall.sh
done

for i in "${triple_bits[@]}"; do
	o1=$( echo "$i" | grep  -Eo "[0-9]+" | head -n 1 )
	o2=$( echo "$i" | grep  -Eo "[0-9]+" | head -n 2 | tail -n 1 )
	o3=$( echo "$i" | grep  -Eo "[0-9]+" | tail -n 1)

	./install.sh test=0 uchar=65 offset=$(( 2 ** $o1 )) offset2=$(( 2 ** $o2 )) offset3=$(( 2 ** $o3 ))
	read -n 1 -p "exported ${i}.csv to /tmp ?" foo
	csv_n=${i}.csv
	scp castor:/tmp/$csv_n .
	awk -F, '$12~/65/{printf("Command #:%s ddr:%s data:%s\n", NR-1 , $8, $12);}'  $csv_n | grep 65
	./uninstall.sh
done

for i in "${single_bits[@]}"; do
	./install.sh test=0 uchar=65 offset=$(( 2 ** $i ))
	read -n 1 -p "exported ${i}.csv to /tmp ?" foo
	csv_n=${i}.csv
	scp castor:/tmp/$csv_n .
	awk -F, '$12~/65/{printf("Command #:%s ddr:%s data:%s\n", NR-1 , $8, $12);}'  $csv_n | grep 65
	./uninstall.sh
done

for i in *.csv; do
	read -n 1 -p "exported ${i}.csv to /tmp ?" fooecho -n "Function: $(echo "$i" | sed 's/\.csv//g'): "
	awk -F, '$12~/61/{printf("Command #:%s ddr:%s data:%s\n", NR-1 , $8, $12);}' $i | grep 61
done
exit
