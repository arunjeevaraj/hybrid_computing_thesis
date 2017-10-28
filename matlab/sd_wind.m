function [ sd_x ] = sd_wind( x, window_size )
% windowed implementation of
if(nargin < 1)
    display('not enough input');
end

alpha = zeros(1, window_size);
beta =  zeros(1, window_size);

N_windows = size(x,2)/window_size;

alpha(1) = x(1);
beta(1) = x(1)^2;


for i = 2: length(window_size)
    alpha(i) = alpha(i-1) + x(i) - x ()
end


end

