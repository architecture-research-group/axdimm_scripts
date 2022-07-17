#!/bin/bash

#1 - hex address
addr_to_offset(){
	echo "obase=10; ibase=16; $1" | bc
}

offset_to_addr(){
	echo "obase=16; ibase=10; $1" | bc
}
