%% Plot Sine
fs = 48000;         % Sampling frequency
ts = 1/fs;          % Time step
dur = 1;            % Duration of the signal in seconds

t = 0:ts:dur-ts;    % Time vector

freq = 1000;        % Frequency of the sine wave
amp = 1;            % Amplitude of the sine wave

sine = amp * sin(2*pi*freq*t);  % Generate the sine wave

figure(1);
plot(t, sine, "LineWidth", 1.5);  % Plot the sine wave
xlim([0, 0.001]);                 % Set the x-axis limits
hold("on");                       % Enable holding the plot for overlaying

sound(sine, fs);                  % Play the sine wave as sound


%% Hard clipping
threshold = 0.5;                  % Threshold for hard clipping

% Apply hard clipping to the sine wave
hcSine = hard_clipper(sine, threshold);

plot(t, hcSine, "LineWidth", 1.5); % Plot the hard clipped signal

sound(hcSine, fs);                % Play the hard clipped signal


%% Soft clipping
scSine = tanh(sine);              % Apply soft clipping using the hyperbolic tangent function

plot(t, scSine, "LineWidth", 1.5); % Plot the soft clipped signal

sound(scSine, fs);                % Play the soft clipped signal


%% Soft clipping with saturation coefficient
saturation = 2;                   % Saturation coefficient

% Apply soft clipping with saturation coefficient to the sine wave
scSineSat = tanh(sine * saturation) / tanh(saturation);

plot(t, scSineSat, "LineWidth", 1.5); % Plot the soft clipped signal with saturation

sound(scSineSat, fs);                % Play the soft clipped signal with saturation


%% Piecewise clip
piecesine = piecewise_clipper(sine);  % Apply a piecewise clipping function

plot(t, piecesine);                   % Plot the piecewise clipped signal

sound(piecesine, fs);                 % Play the piecewise clipped signal


%% Asymmetric hard clip
positive_threshold = 0.5;             % Positive threshold for asymmetric hard clipping
negative_threshold = 0.8;             % Negative threshold for asymmetric hard clipping

% Apply asymmetric hard clipping to the sine wave
asymmHcSine = asymmetric_hard_clipper(sine, positive_threshold, negative_threshold);

plot(t, asymmHcSine, "LineWidth", 1.5); % Plot the asymmetrically hard clipped signal

sound(asymmHcSine, fs);                  % Play the asymmetrically hard clipped signal


%% Symmetric vs Asymmetric hard clip THD
figure(2);

subplot(2,1,1);
thd(hcSine, fs);                    % Compute and plot the THD (Total Harmonic Distortion) for symmetrical clipping

subplot(2,1,2);
thd(asymmHcSine, fs);               % Compute and plot the THD (Total Harmonic Distortion)for asymmetrical clipping
