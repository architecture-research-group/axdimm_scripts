#!/bin/bash

source addresses.sh
[ -z "$offs" ] && offs=( "256" "512" "1024" "2048" "4096" )
off_file=string_table.txt

echo "offset,physical_address,char" >  $off_file

# make sure module is uninstalled before starting
if [ ! -z "$(sudo lsmod | grep rw_test)" ]; then
	sudo rmmod rw_test.ko
fi


for i in "${offs[@]}";
do
	ctr=$(( $ctr + 1 ))
	[[ "$ctr" -gt "25" ]] && ctr=0
	phys=$( ./install.sh \test=1 str=\"TEST$ctr\" offset=$i | grep 'STRING_WRITE'\
	| grep -Eo '\(0x[0-9a-zA-Z]+\):[A-Za-z]+' | tee str.out | grep -Eo '0x[0-9a-zA-Z]+' ) #install
	char=$(grep -Eo ':[A-Z0-9a-z]+' str.out | sed 's/://g')
	echo "${i},${phys},${char}" >> $off_file
	./install.sh #uninstall
done

echo "table in $off_file"

