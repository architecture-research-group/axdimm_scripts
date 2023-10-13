#!/bin/bash
VIVADO_SETTINGS=/tools/xilinx/Vivado/2019.2/settings64.sh
COPY_SCRIPT=./macrobench.sh
SIZE=$(( 2 ** 22 ))
WAYMASK=0x0001
OUTPUT_FILE=/home/a972m888/HPCA_Rebuttal/axdimm_scripts/antagonist_testing/${SIZE}_buffer_${WAYMASK}_Ways_.csv

source $VIVADO_SETTINGS
TCL_SCRIPT=rdcas_wrcas_trace.tcl
vivado -mode tcl -source $TCL_SCRIPT -log vivado.log &
$COPY_SCRIPT $SIZE $WAYMASK
wait

mv /home/a972m888/HPCA_Rebuttal/ilaData/output.csv  $OUTPUT_FILE

