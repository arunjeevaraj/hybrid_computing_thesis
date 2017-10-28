function [ std_deviate ] = sd_welford(x)
%using welford method for computing standard deviation in single run.
m = 0;
s = 0;

for i_x = 1: length(x)
    o_m = m;
    m = m + (x(i_x)-m)/i_x;
    s = s + (x(i_x)-m)*(x(i_x)-o_m);
end
    std_deviate = s/(length(x)-1);
end

