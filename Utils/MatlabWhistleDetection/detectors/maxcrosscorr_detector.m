function  result = maxcrosscorr_detector(capture_data, window_size, window_offset, threshold)

    %% read reference whitle spectrum
    fileID = fopen ('../tests/andyDark_8kHz_1channels.dat', 'r');
    spectrum = fread(fileID, 'double', 'ieee-le');
    fclose(fileID);
    %% Preprocess cpp Spectrum
    spectrum_proc = zeros(2 * window_size, 1);
    %2 * (1024 + 1)
    spectrum_proc(1:2 * (window_size + 1)) = spectrum(1:end - 1);   
    reference_max = spectrum(end);
    
    % convert array of doubles to array of complex values
    reference_spectrum = complex(spectrum_proc(1:2:end),spectrum_proc(2:2:end));
      
    %% Run Max Cross Correlation Detection on a single capture
    result = {};
    result.indices = [];
    result.values  = [];
    for i=1:window_offset:length(capture_data)
        if (i + window_size - 1 > length(capture_data))
            break
        end
        slice = capture_data(i:i + window_size - 1);
        res_slice = maxcrosscorr(slice, reference_spectrum, reference_max);

        % TODO write statistik in capture database
        if(res_slice > threshold)
            disp('Whistle detected')
        end
        result.indices(end + 1) = i;
        result.values(end + 1)  = res_slice;
    end











end