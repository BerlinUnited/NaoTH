% load calculated spectrum
fileID = fopen ('andyDark_8kHz_1channels.dat', 'r');
    cpp_calc_spectrum = fread(fileID, 'double', 'ieee-le');
fclose(fileID);

% get the correlation value and remove it from the array
corr = cpp_calc_spectrum(end);
cpp_calc_spectrum = cpp_calc_spectrum(1:end-1);

% convert array of doubles to array of complex values
cpp_calc_spectrum = complex(cpp_calc_spectrum(1:2:end),cpp_calc_spectrum(2:2:end));

%%
% load raw file
fileID = fopen ('andyDark_8kHz_1channels.raw', 'r');
    raw = fread(fileID, 'int16', 'ieee-le');
fclose(fileID);

fftw('planner','estimate');
%raw = [raw zeros(1024,1)]; % we do this in c++
mat_calc_spectrum = fft(raw, length(raw)*2);

%% Test compare
cpp_calc_spectrum = cpp_calc_spectrum(2:end); % ignore first value

mat_calc_spectrum = conj(mat_calc_spectrum); % since c++ version is conjugated
%normalize
norm_cpp_calc_spectrum = (cpp_calc_spectrum - min(cpp_calc_spectrum)) / ( max(cpp_calc_spectrum) - min(cpp_calc_spectrum) );
norm_mat_calc_spectrum = (mat_calc_spectrum - min(mat_calc_spectrum)) / ( max(mat_calc_spectrum) - min(mat_calc_spectrum) );

norm_mat_calc_spectrum = norm_mat_calc_spectrum(1:1024);
test = norm_mat_calc_spectrum - norm_cpp_calc_spectrum;
figure
plot(abs(norm_mat_calc_spectrum), 'r')
hold on
plot(abs(norm_cpp_calc_spectrum), 'b')

figure
plot(abs(test), 'r')
title('Diff')


