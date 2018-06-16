function result = slidingcrosscorrelation(capture_data, reference, window_size, window_offset,threshold)

    reference_spectrum = reference.spectralData;
    reference_max = reference.autocorrelation;
    
    result.positions = [];
    
    for i=1:window_offset:length(capture_data)
        % dont check the last samples in the data -> possible problem if
        % the whistle is towards the end of the capture
        if (i + window_size - 1 > length(capture_data))
            break
        end
        slice = capture_data(i:i + window_size - 1);
        
        res_slice = maxcrosscorr(slice, reference_spectrum, reference_max);
        
        if(res_slice >= threshold)
            result.detected = true;
            result.positions(end + 1) = i; % window start position in capture data
        else
            result.positions = [];
            result.detected = false;
        end
        
    end
end
