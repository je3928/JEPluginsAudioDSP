function [x_f, x_fft] = fftfrequencyplot(x, fs)
% FFTFREQUENCYPLOT Generates outputs that will plot to create frequency spectrum graph
%
% Pass in signal x and correct sampling frequency fs to generate 2 vectors
% One with calculated amplitude values and another with frequency bins
% The vectors are then plotted on a semilog scale

% Calculate length of the input signal
len = length(x);

% Calculate the next power of two of the signal length. This allows for efficient FFT processing
len = 2 ^ nextpow2(len);

% Calculate the FFT of the signal
ffx = fft(x, len);

% Calculate the single-sideband (SSB) spectrum
SSB = ffx(1:(len/2) + 1);

% Calculate the frequency bins
f = (fs * (0:(len/2)) / len);

% Assign the calculated frequency and SSB spectrum to the output variables
x_f = f;
x_fft = abs(SSB);

% Convert the spectrum to dB scale
x_fft = 20 * log10(x_fft);

% Plot the frequency spectrum using a semilogarithmic scale
semilogx(f, x_fft);
xlim([20 20000]);
ylim([-60 12]);
title('Frequency Plot');
xlabel("Frequency (Hz)");
ylabel("Amplitude (dB)");
grid("on");

end
