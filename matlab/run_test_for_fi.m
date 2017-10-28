% NOTE run_script.m should be run before this script to generate the needed 
% data.

% particle info containers pre-allocation to speed up the code execution.
particle_i_row_size = particle_cnt*6;       % size of each frames
num_of_frames = N_cells*N_transf_per_cell+1;
particle_i = zeros(particle_i_row_size, 1); % initial particle info
particle_info_frames = zeros(particle_i_row_size,...
    num_of_frames);                         % entire particle info
particle_o =(zeros(size(particle_i)));       % last particle frame info.



%find the frames which has maximum standard deviation.
frame_max_sd = zeros(6,1);
for i = 1:6
  frame_max_sd(i) = find(sd_particles(i,:)== max(sd_particles(i,:)), 1, 'last' );
end


% transferring beam_particle data from run_Script.m file to 
% the format the data is stored in OpenCL impelementation.
for j = 1:num_of_frames
    for i = 1: particle_cnt
        particle_info_frames((i-1)*6+1:i*6, j) = beam_particles(:,j,i);
    end
end

i = 1;
particle_i = particle_info_frames(:,i); % initial data with double precision.
i = 2;
particle_o = particle_info_frames(:,i); % expected data with double precision.

dynamic_range = [min(min(particle_info_frames)),...
    max(max(particle_info_frames))]


% there are two methods while the fixed point length is concerned,
% one which uses the fraction length to represent the data, when using fi 
% or
% like in hardware the notion of fractional length can be ignored. but the
% data needs to be normalised from the simulation point of view.

% the latter method is used for the script due to prior familiarity.
fixed_word_length = 32;    % wordlength for beam particle.
scaling_factor = 21;        
fixed_word_length_tm = 24;  % wordlength for transfer matrix.
scaling_factor_tm = fixed_word_length_tm-2;
particle_i_scaled = particle_i * 2^scaling_factor;





% quantized into 32 bit signed precision.
particle_i_fi = fi(particle_i_scaled, 1, fixed_word_length, 0);

particle_i_from_fi = particle_i_fi.double / 2^scaling_factor;

% report the error due to quantization of data to fixed point.
quantization_error = particle_i - particle_i_from_fi;
mqe = max(max(abs(quantization_error)))
quantization_error_mse = sqrt(mean(quantization_error.^2))
figure(1);
plot(quantization_error);
title('quantization error in frame i');
xlabel('particle information for frame i');
ylabel('quantization error');


%quantize the transfer matrices with the same settings.
display('R_drift_quantization results');
R_drift_fi = fi(R_drift * 2^scaling_factor_tm, 1, fixed_word_length_tm, 0);
R_drift_from_fi = R_drift_fi.double / 2^scaling_factor_tm;
quantization_error = R_drift - R_drift_from_fi;
mqe_rdrift = max(max(abs(quantization_error)))
quantization_error_mse_rdrift = sqrt(sum(sum((quantization_error.^2)))/36)

display('R_qfdef quantization results');
R_qfdef_fi = fi(R_qfdef * 2^scaling_factor_tm, 1, fixed_word_length_tm, 0);
R_qfdef_from_fi = R_qfdef_fi.double / 2^scaling_factor_tm;
quantization_error = R_qfdef - R_qfdef_from_fi;
mqe_rqfdef = max(max(abs(quantization_error)))
quantization_error_mse_rqfdef = sqrt(sum(sum((quantization_error.^2)))/36)

display('R_qffoc quantization results');
R_qffoc_fi = fi(R_qffoc * 2^scaling_factor_tm, 1, fixed_word_length_tm, 0);
R_qffoc_from_fi = R_qffoc_fi.double / 2^scaling_factor_tm;
quantization_error = R_qffoc - R_qffoc_from_fi;
mqe_rqffoc = max(max(abs(quantization_error)))
quantization_error_mse_rqffoc = sqrt(sum(sum((quantization_error.^2)))/36)

% model the matrix multiplication for one frame to the next frame.
% the data is finally stored back to 32 bit word length.

%creating a buffer to store the intermediate
%results after multiplication. size - 6*6. wordlentght = 2 * wordlength_i
% 1 bit of 32 bit is used for sign. so, 31 * 2 + 1 is the word length after
% multiplication, if word length of the operands is 32 bit.

buffer_word_length = fixed_word_length +fixed_word_length_tm - 1;
particle_o_fi = fi(zeros(size(particle_i)), 1, fixed_word_length, 0);
particle_o_bq = zeros(size(particle_i));
buffer_mult = fi(zeros(6,6), 1, buffer_word_length, 0);
    for j = 1:particle_cnt
        for i = 1:6
            buffer_mult(:,i) = particle_i_fi(6*(j-1) + i)*R_qffoc_fi(:,i);
        end
        % accumulate the buffer results to store to the output frame.
        accumulated_buff = sum(buffer_mult, 2);
        % removing the lowest digits(scaling factor.)
        acc_buff_normalized = accumulated_buff.double/2^scaling_factor_tm;
        for i = 1:6
            particle_o_fi(6*(j-1) + i) = acc_buff_normalized(i);
            particle_o_bq(6*(j-1) + i) = acc_buff_normalized(i);
        end 
    end


% difference between the expected and the fixed point data types.
display('output frame');
difference_output = (particle_o_fi.double/2^scaling_factor) - particle_o;
mqe = max(max(abs(difference_output)));
mse = sqrt(sum(sum((difference_output.^2)))/length(difference_output))
    
    
% plot the log2 dynamic range of the particle_o;
%figure(2);
h = NumericTypeScope;
release(h);
%hold on;
step(h, particle_o_bq);

% quantize the results back to 32 bit result.