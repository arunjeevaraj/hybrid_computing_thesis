function [ beam ] = Fodo_sliced_sim( beam_init, properties, ...
        R_focus_Ns, R_drift_Ns, R_defocus_Ns)
% ``    R_focus_Ns, R_drift_Ns, R_defocus_Ns);
    N_cells = properties(1);
    N_transf_per_cell = properties(2);
    Ns = properties(3);
    beam_particles = zeros(size(beam_init, 1), ...
        N_cells*N_transf_per_cell + 1, ...
        size(beam_init, 2));       
      for i_cell = 1 : N_cells  
        for i_part = 1 : size(beam_init, 2)
            buffer = beam_init(:, i_part);
            % focus
            for j = 1 : Ns
                buffer = R_focus_Ns * buffer;
            end
            beam_particles(:, i_cell + 1, i_part) = buffer;
            %drift
            for j = 1 : Ns
                buffer = R_drift_Ns * buffer;
            end
            beam_particles(:, i_cell + 2, i_part) = buffer;
            %defocus
            for j = 1 : Ns
                buffer = R_defocus_Ns * buffer;
            end
            beam_particles(:, i_cell + 3, i_part) = buffer;
            %drift
            for j = 1 : Ns
               buffer = R_drift_Ns * buffer;
            end
            beam_particles(:,i_cell + 4, i_part) = buffer;
            %focus
            for j = 1 : Ns
                buffer = R_focus_Ns * buffer;
            end
            beam_particles(:, i_cell + 5, i_part) = buffer;
        end
      end
% output
    beam = beam_particles;
end

