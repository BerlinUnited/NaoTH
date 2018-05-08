function result = zcr_detector(capture_data, window_size, window_offset, threshold)
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
        res_slice = zcr(slice, window_size, 256);

        % TODO write statistik in capture database
        if(res_slice > threshold)
%             disp('Whistle detected by ZCR')
            result.whistle_detected = true;
        end
        result.indices(end + 1) = i;
        result.values(end + 1)  = res_slice;
    end
end
