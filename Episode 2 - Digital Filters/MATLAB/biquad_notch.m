function [y] = biquad_notch(x, fc, fs, Q)
% Implements a biquad notch filter with the given parameters.
% Inputs:
%   - x: Input signal
%   - fc: Center frequency of the notch
%   - fs: Sampling frequency
%   - Q: Quality factor of the filter
% Output:
%   - y: Output signal after filtering

% Calculate angular frequency
w = 2*pi*(fc / fs);

% Compute cosine and sine of the angular frequency
cosw = cos(w);
sinw = sin(w);

% Compute the damping coefficient 'a'
a = sinw / (2*Q);

% Compute the feedforward coefficients 'b'
b0 = 1;
b1 = -2 * cosw;
b2 = 1;

% Compute the feedback coefficients 'a'
a0 = 1 + a;
a1 = -2 * cosw;
a2 = 1 - a;

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
