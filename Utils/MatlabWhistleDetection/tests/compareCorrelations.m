%clear all
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

% load correlation function calculated by cpp
cpp_correlation_func = textread('andy_cpp_correlations.txt', '%f', 'delimiter', '\n');

%% Preprocess cpp Spectrum
cpp_spectrum_proc = cpp_spectrum(1:end-1);
% convert array of doubles to array of complex values
cpp_spectrum_proc = complex(cpp_spectrum_proc(1:2:end),cpp_spectrum_proc(2:2:end));

%% Calculate correlation function in matlab

% normalize the samples like the cpp implementation
short_max = 32767;
raw_samples_norm = raw_samples / short_max;

% Version 1 of correlation calculation
fftw('planner','estimate');
mat_spectrum = fft(raw_samples_norm, length(raw_samples_norm)*2);

intermediate_result = mat_spectrum .* conj(mat_spectrum);
mat_correlation_func = ifft(intermediate_result, length(raw_samples_norm)*2);

% Version 2 - different normalization
[autocorrelation, shift] = xcorr(raw_samples_norm);
[max_value, peak_index] = max(abs(autocorrelation));
t_lag = abs(shift(peak_index));
%% Plot unnormalized Correlation Functions
subplot(3,3,1)
plot(cpp_correlation_func)
title('unnormalized cpp Correlation')
subplot(3,3,2)
plot(mat_correlation_func)
title('unnormalized mat Correlation')
subplot(3,3,3)  
plot(shift, autocorrelation)
title('mat Correlation via xcorr')
%% Plot Normalized Correlation Functions
mat_correlation_func_norm = mat_correlation_func / max(abs(mat_correlation_func));
cpp_correlation_func_norm = cpp_correlation_func / max(abs(cpp_correlation_func));
acor_norm = autocorrelation / max(abs(autocorrelation));

subplot(3,3,4)
plot(cpp_correlation_func_norm)
title('normalized cpp Correlation')
subplot(3,3,5)
plot(mat_correlation_func_norm)
title('normalized mat Correlation')
subplot(3,3,6)
plot(shift, acor_norm)
title('normalized mat Correlation via xcorr')

%% Relationship between mat correlation function and cpp correlation
% http://www.fftw.org/fftw3_doc/The-1d-Discrete-Fourier-Transform-_0028DFT_0029.html#The-1d-Discrete-Fourier-Transform-_0028DFT_0029 
% divide cpp result by n = 2048 or multiply matlab implementation by 2048
% to get the cpp implementation
% assumes that 2048 is length(raw_samples * 2)
mat_correlation_func_norm2 = mat_correlation_func * 2048; 
subplot(3,3,7)
plot(cpp_correlation_func)
title('unnormalized cpp Correlation')
subplot(3,3,8)
plot(mat_correlation_func_norm2)
title('normalized mat Correlation')
%% Check if cpp_max_auto_corr is same as mat_max_auto_corr

cpp_max_auto_corr = max(cpp_correlation_func);
mat_max_auto_corr = max(mat_correlation_func_norm2);
auto_corr_error = abs(mat_max_auto_corr - cpp_max_auto_corr)

