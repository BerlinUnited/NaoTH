clear all
clc
%% Get a reference spectrum
%TODO later get it from a database
fileID = fopen ('andyDark_8kHz_1channels.dat', 'r');
    reference_spectrum = fread(fileID, 'double', 'ieee-le');
fclose(fileID);

%% Preprocess cpp Spectrum
% remove the correlation value from the array
reference_spectrum_proc = reference_spectrum(1:end-1);

% convert array of doubles to array of complex values
reference_spectrum_proc = complex(reference_spectrum_proc(1:2:end),reference_spectrum_proc(2:2:end));

% get a validation file
load('validation_data.mat')
%sound(validation_data, 8000);
audiowrite('validation_data.wav',validation_data,8000)