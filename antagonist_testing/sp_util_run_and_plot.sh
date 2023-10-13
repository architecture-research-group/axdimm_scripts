#!/bin/bash
VIVADO_SETTINGS=/tools/xilinx/Vivado/2019.2/settings64.sh
REMOTE=n869p538@axdimm.ittc.ku.edu
COPY_SCRIPT=/home/n869p538/axdimm_modules/antagonist_testing/macrobench.sh
PLOT_SCRIPT=./Python_AxDIMM_Script/Arbiter_Script.py
SIZE=$(( 2 ** 22 ))
WAYMASK=0x0001
OUTPUT_FILE=$(pwd)/${SIZE}_buffer_${WAYMASK}_Ways_.csv

source $VIVADO_SETTINGS
TCL_SCRIPT=rdcas_wrcas_trace.tcl
vivado -mode tcl -source $TCL_SCRIPT -log vivado.log &
ssh $REMOTE $COPY_SCRIPT $SIZE $WAYMASK 
pid=$!
wait

mv /home/a972m888/HPCA_Rebuttal/ilaData/output.csv  $OUTPUT_FILE

python3 $PLOT_SCRIPT -csv $OUTPUT_FILE
