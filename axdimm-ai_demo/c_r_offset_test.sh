#!/bin/bash

source addresses.sh
[ -z "$offs" ] && offs=( "256" "512" "1024" "2048" "4096" )
off_file=char_table.txt

echo "offset,physical_address,char" >  $off_file

# make sure module is uninstalled before starting
if [ ! -z "$(sudo lsmod | grep rw_test)" ]; then
	./uninstall.sh >/dev/null
fi


for i in "${offs[@]}";
do
	ctr=$(( $ctr + 1 ))
	[[ "$ctr" -gt "25" ]] && ctr=0
	c_phys=$( ./install.sh \test=0 uchar=$(( 65 + $ctr )) offset=$i | grep 'CHAR_WRITE' | tail -n 1 \
	| grep -Eo '\(0x[0-9a-zA-Z]+\):[A-Za-z]+' | tee str.out | grep -Eo '0x[0-9a-zA-Z]+'  ) #install
	w_char=$(grep -Eo ':[A-Z0-9a-z]+' str.out | sed 's/://g')
	./uninstall.sh >/dev/null #uninstall
	r_phys=$( ./install.sh \test=2 rdlen=8 offset=$i | grep 'READ' | tail -n 1 \
	| grep -Eo '\(0x[0-9a-zA-Z]+\):.*' | tee str.out | grep -Eo '0x[0-9a-zA-Z]+' ) #install
	r_char=$(grep -Eo ':.*' str.out | sed 's/://g')
	echo "write,${i},${c_phys},${w_char}" | tee -a $off_file
	echo "read,${i},${r_phys},${r_char}" | tee -a $off_file
	./uninstall.sh >/dev/null #uninstall
done

echo "table in $off_file"
