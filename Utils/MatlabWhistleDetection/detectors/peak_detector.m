
function result = peak_detector(capture_data, window_size, window_offset, threshold)
    %% Run Peak Detection on a single capture
    result = {};
    result.indices = [];
    result.values  = [];
    result.whistle_detected = false;
    for i=1:window_offset:length(capture_data)
        if (i + window_size - 1 > length(capture_data))
            break
        end
        slice = capture_data(i:i + window_size - 1);
        max_slice = max(slice);
        
        % TODO write statistik in capture database
        if(max_slice > threshold)
%             disp('Whistle detected by max peak')
            result.whistle_detected = true;
        end
        result.indices(end + 1) = i;
        result.values(end + 1)  = max_slice;
    end
end