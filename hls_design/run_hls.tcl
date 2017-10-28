# *******************************************************************************
# Author: Arun Jeevaraj
# Associated Filename: run_hls.tcl
# Purpose: Tcl commands to setup a Vivado HLS prject
# Device: Xilinx FPGA devices.
# Revision History: 2017 initial version.
# Copyright: All rights reserved.
##############################################
# Project settings

# Create a project
open_project	-reset matrix_dut_prj

# The source file and test bench
add_files			dut.cpp
add_files -tb	dut_test.cpp
add_files -tb Beam_init.bin
add_files -tb Beam_end.bin
add_files -tb R_drift.bin
add_files -tb R_qfdef.bin
add_files -tb R_qffoc.bin
# Specify the top-level function for synthesis
set_top				dut

###########################
# Solution settings

# Create solution1
open_solution -reset solution1

# Specify a Xilinx device and clock period
# - Do not specify a clock uncertainty (margin)
# - Let the  margin to default to 12.5% of clock period
set_part  {xcku035-fbva900-2-e} -tool vivado
create_clock -period 8
set_clock_uncertainty 1.25

# Simulate the C code
csim_design

# Exit the Vivado Instance.
exit
