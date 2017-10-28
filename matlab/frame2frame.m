%script to study frame2frame error accumulation.
particle_i_row_size = particle_cnt*6;
num_of_frames = N_cells*N_transf_per_cell+1;
particle_info_frames = zeros(particle_i_row_size, num_of_frames);  
mse_frames = zeros(1, num_of_frames);
for j = 1:num_of_frames
    for i = 1: particle_cnt
        particle_info_frames((i-1)*6+1:i*6, j) = beam_particles(:,j,i);
    end
end


fixed_word_length = 32+16;    % wordlength for beam particle.
scaling_factor = 21+16;        
fixed_word_length_tm = 24+16;  % wordlength for transfer matrix.
scaling_factor_tm = fixed_word_length_tm-2;

particle_info_frames_fi = fi(zeros(size(particle_info_frames)), 1, ...
    fixed_word_length, 0);

% get the initial state to fixed point.
i=1;
particle_i_scaled = particle_info_frames(:,i) * 2^scaling_factor;
particle_i_fi = fi(particle_i_scaled, 1, fixed_word_length, 0);


% get the transfer matrices to the fixed point space.
R_drift_fi = fi(R_drift * 2^scaling_factor_tm, 1, fixed_word_length_tm, 0);
R_qfdef_fi = fi(R_qfdef * 2^scaling_factor_tm, 1, fixed_word_length_tm, 0);
R_qffoc_fi = fi(R_qffoc * 2^scaling_factor_tm, 1, fixed_word_length_tm, 0);
current_trans_mat = fi(zeros(size(R_qffoc)), 1, fixed_word_length_tm, 0);

% to store intermediate results during the multiplication.
buffer_word_length = fixed_word_length + fixed_word_length_tm - 1;
buffer_mult = fi(zeros(6,6), 1, buffer_word_length, 0);

% find the second frame for the particle beam.
for j = 1:particle_cnt
    for i = 1:6
        buffer_mult(:,i) = particle_i_fi(6*(j-1) + i)*R_qffoc_fi(:,i);
    end
    accumulated_buff = sum(buffer_mult, 2);
    % removing the lowest digits(scaling factor.)
    acc_buff_normalized = accumulated_buff.double/2^scaling_factor_tm;
    for i = 1:6
       particle_info_frames_fi(6*(j-1) + i,2) = acc_buff_normalized(i);
    end 
end
p_o_fi = particle_info_frames_fi(:,2);
difference_output = (p_o_fi.double/2^scaling_factor)...
    - particle_info_frames(:,2);
mse_frames(2) = sqrt(sum(sum((difference_output.^2)))/length(difference_output));

%iterate the beam over the total number of frames.
for k = 2:num_of_frames-1
    k
    % select the current trans_mat
    if( mod(k,5) == 2 || mod(k,5) == 4 )
        current_trans_mat = R_drift_fi;
    elseif(mod(k,5) == 3)
        current_trans_mat = R_qfdef_fi;
    elseif(mod(k,5) == 1 || mod(k,5) == 0)
        current_trans_mat = R_qffoc_fi;
    end
    
    for j = 1:particle_cnt
        for i = 1:6
            buffer_mult(:,i) = particle_info_frames_fi(6*(j-1) + i,k)...
                *current_trans_mat(:,i);
        end
        accumulated_buff = sum(buffer_mult, 2);
        % removing the lowest digits(scaling factor.)
        acc_buff_normalized = accumulated_buff.double/2^scaling_factor_tm;
        for i = 1:6
            particle_info_frames_fi(6*(j-1) + i,k + 1) = acc_buff_normalized(i);
        end 
    end
    % find mean square error.
    p_o_fi = particle_info_frames_fi(:,k + 1);
    d_o = (p_o_fi.double/2^scaling_factor)...
    - particle_info_frames(:,k + 1);
    mse_frames(k + 1) = sqrt(sum(sum((d_o.^2)))/length(d_o));
end

%stem the mean square of each frame 
%figure(3)
%stem(mse_frames);
%title('Mean Square Error at each frame vs Frame index.');
%xlabel('Frame index');
%ylabel('Mean Square Error');
hr = figure(4);
loglog(mse_frames,'-s');
title('Error Accumulation due to Iterative Nature.');
xlabel('log(Frame index,10)');
ylabel('log(Mean Square Error,10)');
%semilogx(mse_frames);
%title('Semilog plot of Mean Square Error at each frame vs Frame index.');
%xlabel('log(Frame index,10)');
%ylabel('Mean Square Error');
print(hr,'-dpdf','error_accum.pdf', '-opengl')