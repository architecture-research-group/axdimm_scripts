#!/bin/bash

offs=( "256" "512" "1024" "2048" "4096" )
off_file=offset_table.txt

echo "offset,physical_address,char" >  $off_file

for i in "${offs[@]}";
do
	ctr=$(( $ctr + 1 ))
	[[ "$ctr" -gt "25" ]] && ctr=0
	phys=$( ./install.sh \test=0 uchar=$(( 65 + $ctr )) offset=$i | grep 'phys'\
	| grep -Eo '\(0x[0-9]+\):[A-Za-z]+' | tee str.out | grep -Eo '0x[0-9]+' ) #install
	char=$(grep -Eo '[A-Z]+' str.out)
	echo "${i},${phys},${char}" >> $off_file
	./install.sh #uninstall
done

echo "table in $off_file"
