clear all
clc
% load calculated spectrum
fileID = fopen ('data/andyDark_8kHz_1channels.dat', 'r');
    cpp_calc_spectrum = fread(fileID, 'double', 'ieee-le');
fclose(fileID);

% get the correlation value and remove it from the array
cpp_auto_corr = cpp_calc_spectrum(end);
cpp_calc_spectrum = cpp_calc_spectrum(1:end-1);

% convert array of doubles to array of complex values
cpp_calc_spectrum = complex(cpp_calc_spectrum(1:2:end),cpp_calc_spectrum(2:2:end));

%%
% load raw file
fileID = fopen ('data/andyDark_8kHz_1channels.raw', 'r');
    raw = fread(fileID, 'int16', 'ieee-le');
fclose(fileID);

% normalize the samples like the cpp implementation
short_max = 32767;
raw = raw / short_max;

fftw('planner','estimate');
mat_calc_spectrum = fft(raw, length(raw)*2);

%% Test IFFT
fftw('planner','estimate');
new_raw = ifft(mat_calc_spectrum, length(raw)*2);

figure
plot(abs(raw) , 'r')
hold on
plot(abs(new_raw), 'b')
title('Raw Values and ...')

%% Test xcorr vs correlation via fft/ifft

%% Test compare
mat_calc_spectrum = conj(mat_calc_spectrum); % since c++ version is conjugated

% only use half of the spectrum
mat_calc_spectrum = mat_calc_spectrum(1:1025);
%%
% this cross correlation does not compute the same value as the c++
% implementation
[acor,lag] = xcorr(raw, raw);
[value, index] = max(abs(acor));
%lagDiff = lag(index);

%%
% figure
% plot(abs(mat_calc_spectrum) , 'r')
% hold on
% plot(abs(cpp_calc_spectrum), 'b')
% 
% diff = mat_calc_spectrum - cpp_calc_spectrum;
% 
% figure
% plot(abs(diff), 'r')
% title('Differenz')


