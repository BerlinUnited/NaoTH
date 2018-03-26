function [spectrum, auto_corr] = calculate_spectrum(raw)
%CALCULATE_SPECTRUM Summary of this function goes here
%   Detailed explanation goes here

mat_calc_spectrum = fft(raw, length(raw)*2);
mat_calc_spectrum = conj(mat_calc_spectrum); % since c++ version is conjugated

% normalize like the cpp implementation
short_max = 32767;
norm_mat_calc_spectrum = mat_calc_spectrum / short_max;
norm_mat_calc_spectrum = norm_mat_calc_spectrum(1:1025);

% TODO calculate auto correlation
[acor,lag] = xcorr(norm_mat_calc_spectrum, norm_mat_calc_spectrum);
[~,I] = max(abs(acor))
lagDiff = lag(I);

spectrum = norm_mat_calc_spectrum;
auto_corr = inputArg2;
end

