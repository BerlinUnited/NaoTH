function [spectrum, max_auto_corr] = calc_spectrum(raw_samples)

    % normalize the raw samples like the cpp implementation
    short_max = 32767;
    raw_samples_norm = raw_samples / short_max;

    fftw('planner','estimate');
    mat_spectrum = fft(raw_samples_norm, length(raw_samples_norm)*2);

    mat_spectrum_conj = conj(mat_spectrum); % since c++ version is conjugated

    intermediate_result = mat_spectrum .* mat_spectrum_conj;
    mat_correlation_func = ifft(intermediate_result, length(raw_samples_norm)*2);

    % unnormalize it because cpp implementation expects it that way
    % assumes that 2048 is length(raw_samples * 2)
    mat_correlation_func = mat_correlation_func * 2048;
    disp(length(mat_correlation_func))
    % only output half of the spectrum
    spectrum = mat_spectrum_conj(1:1025);
    max_auto_corr = max(mat_correlation_func);
end

