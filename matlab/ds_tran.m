function [ R_drift ] = ds_tran( L, beta, gama, Ns )
% creates the transfer matrix for drift space given the parameters.
L = L/Ns;
% setup transfer matrix for drift space.
R_drift = eye(6);
R_drift(1,2) = L;
R_drift(3,4) = L;
R_drift(5,6) = L/(beta^2*gama^2);
end

