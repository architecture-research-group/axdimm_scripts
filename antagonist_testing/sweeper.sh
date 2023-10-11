#!/bin/bash
sudo pqos -R
sudo pqos -e "llc:0=0x00FF"
sudo pqos -a "llc:0=0-10"

taskset -c 0 sudo ./sweeper
