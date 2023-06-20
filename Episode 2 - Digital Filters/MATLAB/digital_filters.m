%% Import impulse response

% fs = 44100
% Read the audio file containing the impulse response and its sample rate
[Impulse, fs] = audioread("deltafunction.wav");

% Mono impulse required, take one channel
Impulse = Impulse(:, 1);

% Normalize the impulse response
Impulse = Impulse / max(Impulse);


%% Low Pass Filter

% Cutoff frequency
fc = 500;

% Quality factor
Q = 0.707;

% Apply a biquad low-pass filter to the impulse response
BiquadLP_IR = biquad_LPF(Impulse, fc, fs, Q);

% Plot impulse response of filter
figure(1);
fftfrequencyplot(BiquadLP_IR, fs);


%% High Pass Filter

% Apply a biquad high-pass filter to the impulse response
BiquadHP_IR = biquad_HPF(Impulse, fc, fs, Q);

% Plot impulse response of filter
fftfrequencyplot(BiquadHP_IR, fs);


%% Notch

% Apply a biquad notch filter to the impulse response
BiquadNotch_IR = biquad_notch(Impulse, fc, fs, Q);

% Plot impulse response of filter
fftfrequencyplot(BiquadNotch_IR, fs);


%% Peaking

% Specify the gain in decibels
gain_dB = 6;

% Apply a biquad peaking filter to the impulse response
BiquadPeak_IR = biquad_peaking(Impulse, fc, fs, gain_dB, Q);

% Plot impulse response of filter
fftfrequencyplot(BiquadPeak_IR, fs);


%% Shelving Filters boost

% Specify the quality factor
Q = 0.707;

% Specify the low and high cutoff frequencies for shelving filters
lowfc = 100;
highfc = 3000;

% Apply a low-shelf filter and then a high-shelf filter to the impulse response
ShelvingIR = biquad_lowshelf(Impulse, lowfc, fs, gain_dB, Q);
ShelvingIR = biquad_highshelf(ShelvingIR, highfc, fs, gain_dB, Q);

% Plot impulse response of filter
fftfrequencyplot(ShelvingIR, fs);


%% Shelving Filters cut

% Specify the gain in decibels for cutting
gain_dB = -6;

% Apply a low-shelf filter and then a high-shelf filter to the impulse response
ShelvingIR = biquad_lowshelf(Impulse, lowfc, fs, gain_dB, Q);
ShelvingIR = biquad_highshelf(ShelvingIR, highfc, fs, gain_dB, Q);

% Plot impulse response of filter
fftfrequencyplot(ShelvingIR, fs);
