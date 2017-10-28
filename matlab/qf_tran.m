function [ R_quad ] = qf_tran( L, W, beta, gama, focus, Ns )
% in focused mode along x, the W value should be positive.
L = L/Ns;
if(focus) % if the magnet is focusing along x axis and defocusing along y.
  R_quad = [  cos(W*L)   sin(W*L)/W  0          0         0            0;
             -W*sin(W*L) cos(W*L)    0          0         0            0;
              0           0       cosh(W*L)  sinh(W*L)/W  0            0;
              0           0       W*sinh(W*L)  cosh(W*L)  0            0;
              0           0          0          0         1   L/(beta^2*gama^2);
              0           0          0          0         0            1; ];   
else % if the magnet is focusing along y axis and defocusing along x.
    % W value should be -ve.
 R_quad = [  cosh(W*L)   sinh(W*L)/W  0          0         0            0;
             W*sinh(W*L)  cosh(W*L)   0          0         0            0;
             0           0        cos(W*L)   sin(W*L)/W    0            0;
             0           0       -W*sin(W*L) cos(W*L)      0            0;
             0           0          0          0           1  L/(beta^2*gama^2);
             0           0          0          0           0            1; ];
end
end

