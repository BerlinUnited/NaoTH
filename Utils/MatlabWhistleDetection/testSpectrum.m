fileID = fopen ('andyDark_8kHz_1channels.dat', 'r');
    cpp_calc_spectrum = fread(fileID, 'double', 'ieee-le');
fclose(fileID);

% get the correlation value and remove it from the array
corr = cpp_calc_spectrum(end);
cpp_calc_spectrum = cpp_calc_spectrum(1:end-1);

% convert array of doubles to array of complex values
%%
cpp_calc_spectrum = complex(cpp_calc_spectrum(1:2:end),cpp_calc_spectrum(2:2:end));

% TODO visualize
plot(cpp_calc_spectrum, '.') % complex plane