function  results = maxcrosscorr_detector(capture_data, window_size, window_offset, threshold, ref)

    results = struct;
    for r = 1:length(ref)
        reference_spectrum = ref(r).spectralData;
        reference_max = ref(r).autocorrelation;
  
        % Run Max Cross Correlation Detection on a single capture
        result = struct;
        result.name = ref(r).name;
        result.indices = [];
        result.values  = [];
        result.whistle_detected = false;
        for i=1:window_offset:length(capture_data)
            if (i + window_size - 1 > length(capture_data))
                break
            end
            slice = capture_data(i:i + window_size - 1);
            res_slice = maxcrosscorr(slice(end:-1:1), reference_spectrum, reference_max);

            % TODO write statistik in capture database
            if(res_slice >= threshold)
%                 disp(strcat('Whistle detected by: ', result.name))
                result.whistle_detected = true;
            end
            result.indices(end + 1) = i;
            result.values(end + 1)  = res_slice;
        end
        if r == 1
            results = result;
        else
            results = [results result];
        end
    end

end