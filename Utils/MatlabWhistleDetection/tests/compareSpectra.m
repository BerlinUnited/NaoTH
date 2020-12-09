clear all
clc
%% Load data
% load calculated spectrum
fileID = fopen ('andyDark_8kHz_1channels.dat', 'r');
    cpp_spectrum = fread(fileID, 'double', 'ieee-le');
fclose(fileID);

% load raw file
fileID = fopen ('andyDark_8kHz_1channels.raw', 'r');
    raw_samples = fread(fileID, 'int16', 'ieee-le');
fclose(fileID);
%% Preprocess cpp Spectrum
% remove the correlation value from the array
cpp_spectrum_proc = cpp_spectrum(1:end-1);

% convert array of doubles to array of complex values
cpp_spectrum_proc = complex(cpp_spectrum_proc(1:2:end),cpp_spectrum_proc(2:2:end));

%% Calculate spectrum in matlab

% normalize the raw samples like the cpp implementation
short_max = 32767;
raw_samples_norm = raw_samples / short_max;

fftw('planner','estimate');
mat_spectrum = fft(raw_samples_norm, length(raw_samples_norm)*2);


%% Test compare
mat_spectrum = conj(mat_spectrum); % since c++ version is conjugated

% only use half of the spectrum
mat_spectrum = mat_spectrum(1:1025);

subplot(3,1,1)
plot(abs(mat_spectrum) , 'g')
subplot(3,1,2)
plot(abs(cpp_spectrum_proc), 'b')

diff = abs(mat_spectrum - cpp_spectrum_proc);

subplot(3,1,3)
plot(diff, 'r')
title('Differenz')


