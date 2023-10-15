#!/bin/bash
VIVADO_SETTINGS=/tools/xilinx/Vivado/2019.2/settings64.sh
OUTPUT_FILE=/home/a972m888/HPCA_Rebuttal/axdimm_scripts/antagonist_testing/${SIZE}_buffer_${WAYMASK}_Ways_.csv

source $VIVADO_SETTINGS
TCL_SCRIPT=rdcas_wrcas_trace.tcl
vivado -mode tcl -source $TCL_SCRIPT -log vivado.log &

mv /home/a972m888/HPCA_Rebuttal/ilaData/output.csv  $OUTPUT_FILE

