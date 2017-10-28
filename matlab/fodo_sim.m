function [ beam ] = fodo_sim( beam_init, properties, ...
                             R_qffoc, R_qfdef, R_drift)
    N_cells = properties(1);
    N_transf_per_cell = properties(2);
%runs the fodo simulation.
    disp('Running simulation for Fodo structure.');
    %R_qffoc = qf_tran(L/2, K, beta, gama, 1, 1);
    %R_qfdef = qf_tran(L, -K, beta, gama, 0, 1);
    %R_drift = ds_tran(L, beta, gama, 1);
    det_qff = det(R_qffoc);
    det_qfd = det(R_qfdef);
    det_dss = det(R_drift);
    beam_particles = zeros(size(beam_init, 1), ...
        N_cells*N_transf_per_cell+1, size(beam_init, 2) );
    beam_particles(:,1,:)= beam_init;
    for i_par = 1:size(beam_init, 2)
        beam_particles(:,2,i_par) = R_qffoc*beam_particles(:,1,i_par);       % focus
        beam_particles(:,3,i_par) = R_drift*beam_particles(:,2,i_par);       % drift space
        beam_particles(:,4,i_par) = R_qfdef*beam_particles(:,3,i_par);       % defocus
        beam_particles(:,5,i_par) = R_drift*beam_particles(:,4,i_par);       % drift space
        beam_particles(:,6,i_par) = R_qffoc*beam_particles(:,5,i_par);       % focus.
        for cell_i = 1: N_cells-1
            beam_particles(:,cell_i*5+2,i_par) = R_qffoc*beam_particles(:,cell_i*5+1,i_par); % focus
            beam_particles(:,cell_i*5+3,i_par) = R_drift*beam_particles(:,cell_i*5+2,i_par); % drift space
            beam_particles(:,cell_i*5+4,i_par) = R_qfdef*beam_particles(:,cell_i*5+3,i_par); % defocus
            beam_particles(:,cell_i*5+5,i_par) = R_drift*beam_particles(:,cell_i*5+4,i_par); % drift space
            beam_particles(:,cell_i*5+6,i_par) = R_qffoc*beam_particles(:,cell_i*5+5,i_par); % focus.
        end
    end
    beam = beam_particles;
end

