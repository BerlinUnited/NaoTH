clear variables;
clc;
addpath(genpath('../MatDatabase'))
%% 1. read spectral data
fileID = fopen ('andyDark_8kHz_1channels.dat', 'r');
    original_spectrum = fread(fileID, 'double', 'ieee-le');
    % remove the correlation value from the array
    original_max_auto_corr = original_spectrum(end);
    original_spectrum = original_spectrum(1:end-1);
    % convert array of doubles to array of complex values
    original_spectrum = complex(original_spectrum(1:2:end),original_spectrum(2:2:end));
fclose(fileID);

%% 2. save spectral data
fileID = fopen ('saved_andyDark_8kHz_1channels.dat', 'w');
    real_part = real(original_spectrum);
    imag_part = imag(original_spectrum);
    % interleave the real and imaginary part for output
    output = interleave(real_part, imag_part);
    % append max_auto_corr
    output = [output original_max_auto_corr];
    fwrite(fileID, output, 'double', 'ieee-le');
fclose(fileID);

%% 3. read again
fileID = fopen ('saved_andyDark_8kHz_1channels.dat', 'r');
    saved_spectrum = fread(fileID, 'double', 'ieee-le');
    % remove the correlation value from the array
    saved_max_auto_corr = saved_spectrum(end);
    saved_spectrum = saved_spectrum(1:end-1);
    % convert array of doubles to array of complex values
    saved_spectrum = complex(saved_spectrum(1:2:end),saved_spectrum(2:2:end));
fclose(fileID);

%% 4. Compare the spectra
subplot(3,2,1)
plot(real(original_spectrum) , 'g')
title('Real Original Spectrum')
subplot(3,2,2)
plot(imag(original_spectrum) , 'g')
title('Imaginary Original Spectrum')

subplot(3,2,3)
plot(real(saved_spectrum), 'b')
title('Real Saved Spectrum')
subplot(3,2,4)
plot(imag(saved_spectrum), 'b')
title('Imaginary Saved Spectrum')

diff_real = abs(real(original_spectrum) - real(saved_spectrum));
diff_imag = imag(real(original_spectrum) - imag(saved_spectrum));

subplot(3,2,5)
plot(diff_real, 'r')
title('Differenz Real Part')

subplot(3,2,6)
plot(diff_imag, 'r')
title('Differenz Imaginary Part')