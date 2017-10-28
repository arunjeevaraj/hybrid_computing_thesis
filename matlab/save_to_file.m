% after run_script, to generate the golden reference data.
% for vivado hls test.
num_of_frames = N_cells*N_transf_per_cell+1;
particle_i_row_size = particle_cnt*6;
particle_info_frames = zeros(particle_i_row_size,num_of_frames);

%transforms beam to a frame to frame data structure.
for j = 1:num_of_frames
    for i = 1: particle_cnt
        particle_info_frames((i-1)*6+1:i*6, j) = beam_particles(:,j,i);
    end
end


%to save the transfer matrices.
fid_dm  = fopen('R_drift.bin','w');
fid_qfm = fopen('R_qffoc.bin','w');
fid_qdm = fopen('R_qfdef.bin','w');
%to save the particle beam to file.
fid_in_cond = fopen('Beam_init.bin','w');
fid_out = fopen('Beam_end.bin','w');

% save the transfer matrices.
fwrite(fid_dm, R_drift, 'double');
fwrite(fid_qfm, R_qffoc, 'double');
fwrite(fid_qdm, R_qfdef, 'double');

%save the intial frame and second frame of the beam.
fwrite(fid_in_cond, particle_info_frames(:,1), 'double');
fwrite(fid_out, particle_info_frames(:,end), 'double');


%close the file id.
fclose(fid_dm);
fclose(fid_qfm);
fclose(fid_qdm);
fclose(fid_in_cond);

display('File write script exiting');