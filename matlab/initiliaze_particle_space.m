function[init_beam] = initiliaze_particle_space(N)
%parameters for initial particle distribution.
if(nargin < 1)
    N = 1000;
end
alpha_x = 3;
beta_x = 5;
emittance_x = 1e-4;
gamma_x = (1+alpha_x^2)/beta_x;

alpha_y = -3;
beta_y = 5;
emittance_y = 1e-4;
gamma_y = (1+alpha_y^2)/beta_y;

% number of particles
%N=1000;

normM = [ sqrt(beta_x/emittance_x), 0, 0, 0;
          -alpha_x*sqrt(1/(emittance_x*beta_x)), sqrt(1/(emittance_x*beta_x)), 0 ,0;
          0, 0, sqrt(beta_y/emittance_y), 0;
          0, 0, -alpha_y*sqrt(1/(emittance_y*beta_y)), sqrt(1/(emittance_y*beta_y))
        ];
% creating a circular spatial distribution.
r = sqrt(rand(1,N));
theta = 2*pi*rand(1,N);
x = r.*cos(theta);
px = r.*sin(theta);
r = sqrt(rand(1,N));
theta = 2*pi*rand(1,N);
y=r.*cos(theta);
py=r.*sin(theta);

% beam space 2D
beam = zeros(4,N);
init_beam = beam;
beam(1,:)= x;
beam(2,:)= px;
beam(3,:)= y;
beam(4,:)= py;


%init_beam
for i = 1: N
    init_beam(:,i) = normM*beam(:,i);
end
%stem(beam(1,:), init_beam(3,:));
end