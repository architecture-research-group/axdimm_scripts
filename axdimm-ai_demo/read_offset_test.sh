#!/bin/bash

source addresses.sh
[ -z "$offs" ] && offs=( "256" "512" "1024" "2048" "4096" )
off_file=read_table.txt

echo "offset,physical_address,char" >  $off_file

# make sure module is uninstalled before starting
if [ ! -z "$(sudo lsmod | grep rw_test)" ]; then
	./install.sh
fi


for i in "${offs[@]}";
do
	ctr=$(( $ctr + 1 ))
	[[ "$ctr" -gt "25" ]] && ctr=0
	phys=$( ./install.sh \test=2 rdlen=8 offset=$i | grep 'READ'\
	| grep -Eo '\(0x[0-9a-zA-Z]+\):.*' | tee str.out | grep -Eo '0x[0-9a-zA-Z]+' ) #install
	char=$(grep -Eo ':.*' str.out | sed 's/://g')
	echo "${i},${phys},${char}" >> $off_file
	./install.sh #uninstall
done

echo "table in $off_file"


