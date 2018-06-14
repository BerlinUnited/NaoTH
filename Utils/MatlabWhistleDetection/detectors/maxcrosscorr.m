function result = maxcrosscorr(raw_samples, reference_spectrum, reference_max)

    %% Calculate correlation function in matlab
    
    % normalize the samples like the cpp implementation
    short_max = 32767;
    raw_samples_norm = raw_samples / short_max;

    n = 2^nextpow2(size(raw_samples_norm, 1));
    
    mat_spectrum = fft(raw_samples_norm, n * 2);

    % // real x real - imag x imag
    % fftIn[j][0] = realFFTIn * realFFTCmp - imagFFTIn * imagFFTCmp;
    % // real x imag + imag x real
    % fftIn[j][1] = realFFTIn * imagFFTCmp + imagFFTIn * realFFTCmp;
    intermediate_result = mat_spectrum(1:1025) .* reference_spectrum;
    mat_correlation_func = ifft(intermediate_result, length(raw_samples_norm)*2) * 2048;
    result = max(abs(mat_correlation_func)) / reference_max;
end
