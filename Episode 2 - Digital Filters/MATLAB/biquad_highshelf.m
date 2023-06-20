function [y] = biquad_highshelf(x, fc, fs, gain_dB, Q)
% Implements a biquad high-shelf filter with the given parameters.
% Inputs:
%   - x: Input signal
%   - fc: Cutoff frequency of the high-shelf filter
%   - fs: Sampling frequency
%   - gain_dB: Gain in decibels
%   - Q: Quality factor of the filter
% Output:
%   - y: Output signal after filtering

% Convert the gain from decibels to linear scale
A = 10^(gain_dB/40);

% Calculate angular frequency
w = 2*pi*(fc / fs);

% Compute cosine and sine of the angular frequency
cosw = cos(w);
sinw = sin(w);

% Compute the damping coefficient 'a'
a = sinw / (2*Q);

% Compute intermediate variables
var2sqAa = 2 * sqrt(A) * a;

% Compute the feedforward coefficients 'b'
b0 = A * ((A + 1) + (A - 1) * cosw + var2sqAa);
b1 = -2 * A * ((A - 1) + (A + 1) * cosw);
b2 = A * ((A + 1) + (A - 1) * cosw - var2sqAa);

% Compute the feedback coefficients 'a'
a0 = (A + 1) - (A - 1) * cosw + var2sqAa;
a1 = 2 * ((A - 1 ) - (A + 1) * cosw);
a2 = (A + 1) - (A - 1) * cosw - var2sqAa;

% Initialize variables to store previous input and output samples
xminusone = 0;
xminustwo = 0;
yminusone = 0;
yminustwo = 0;

% Process each sample of the input signal
for i = 1:length(x)
    
    % Calculate the current output sample
    y(i) = (b0 / a0) * x(i) + (b1 / a0) * xminusone + (b2 / a0) * xminustwo - (a1 / a0) * yminusone - (a2 / a0) * yminustwo;

    % Update input history variables
    xminustwo = xminusone;
    xminusone = x(i);

    % Update output history variables
    yminustwo = yminusone;
    yminusone = y(i);
end

end
