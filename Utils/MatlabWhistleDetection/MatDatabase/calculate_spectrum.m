function [spectrum, auto_corr] = calculate_spectrum(raw)
%CALCULATE_SPECTRUM Summary of this function goes here
%   Detailed explanation goes here

mat_calc_spectrum = fft(raw, length(raw)*2);
mat_calc_spectrum = conj(mat_calc_spectrum); % since c++ version is conjugated

% TODO normalize correctly?
norm_mat_calc_spectrum = mat_calc_spectrum;
norm_mat_calc_spectrum = norm_mat_calc_spectrum(1:1025);

% TODO calculate auto correlation

spectrum = norm_mat_calc_spectrum;
auto_corr = inputArg2;
end

