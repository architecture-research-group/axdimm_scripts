#!/bin/bash
VIVADO_SETTINGS=/tools/xilinx/Vivado/2019.2/settings64.sh

source $VIVADO_SETTINGS
TCL_SCRIPT=rdcas_wrcas_trace.tcl
vivado -mode tcl -source $TCL_SCRIPT -log vivado.log
