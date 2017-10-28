############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2017 Xilinx, Inc. All Rights Reserved.
############################################################
set_directive_pipeline "read_data/Feat_Loop_read"
set_directive_pipeline "read_tm/Col_Loop_read"
set_directive_pipeline "write_data/Feat_Loop_write"
set_directive_array_partition -type complete -dim 1 "frame_mul" buffer_res
set_directive_array_partition -type complete -dim 1 "frame_mul" buffer_acc
set_directive_pipeline "frame_mul/PCNT_Loop_mul"
set_directive_interface -mode s_axilite "dut"
set_directive_array_partition -type complete -dim 2 "dut" frame_2d_in
set_directive_array_partition -type complete -dim 2 "dut" frame_2d_out
set_directive_array_partition -type complete -dim 1 "dut" buff_trans_mat
set_directive_dataflow "dut"
set_directive_interface -mode axis -register -register_mode both "dut" frame_i
set_directive_interface -mode axis -register -register_mode both "dut" trans_mat
set_directive_interface -mode axis -register -register_mode both "dut" frame_o
