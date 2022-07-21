#!/bin/bash

source addresses.sh
[ -z "$offs" ] && offs=( "256" "511" )
off_file=rw_table.txt
[ -z "$string" ] && string=ABCDEFG


echo "offset,physical_address,char" >  $off_file

# make sure module is uninstalled before starting
if [ ! -z "$(sudo lsmod | grep rw_test)" ]; then
	./uninstall.sh >/dev/null #uninstall
fi


for i in "${offs[@]}";
do
	ctr=$(( $ctr + 1 ))
	[[ "$ctr" -gt "25" ]] && ctr=0
	./install.sh \test=3 str=\"$string\" offset=$i rdlen=7 | tee strout.txt >/dev/null #install
	w_phys=$( cat strout.txt | grep -e 'STRING_WRITE' | tail -n 1 \
	| grep -Eo '\(0x[0-9a-zA-Z]+\):.+' | tee str.out | grep -Eo '0x[0-9a-zA-Z]+')
	w_str=$(grep -Eo ':.+' str.out | sed 's/://g')
	r_phys=$( cat strout.txt | grep -e 'STRING_READ' | tail -n 1 \
	| grep -Eo '\(0x[0-9a-zA-Z]+\):.+' | tee str.out | grep -Eo '0x[0-9a-zA-Z]+')
	r_str=$(grep -Eo ':.+' str.out | sed 's/://g')
	echo "write,${i},${w_phys},${w_str}" | tee -a $off_file
	echo "read,${i},${r_phys},${r_str}" | tee -a $off_file
	./uninstall.sh >/dev/null #uninstall
done

echo "table in $off_file"

