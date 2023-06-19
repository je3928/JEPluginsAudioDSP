%% Plot non-linear function against ramp to generate transfer function

% Calculate input ramp, which be the input to our transfer function
ramp = -1:0.1:1;

% Apply distortion algorithm to the ramp, change this with distortion
% algorithm being tested
distortion = asymmetric_tanh(ramp, 3);

% Plot transfer function, dont forget to rename title if you change the
% clipper being used. 
plot(ramp, distortion);
title('Tanh clipper transfer function, only applied to top half of signal.');

