function  results = devil_detection(capture_data)
    % parameters from nao devils
    WINDOW_SIZE = 1024;

    MIN_FREQ = 2000;
    MAX_FREQ = 3500;
    MIN_AMP = 8;

    OVERTONE_MULT_MIN_1 = 2;
    OVERTONE_MULT_MAX_1 = 2.1;
    OVERTONE_MIN_AMP_1 = 0.05;

    OVERTONE_MULT_MIN_2 = 3.0;
    OVERTONE_MULT_MAX_2 = 3.2;
    OVERTONE_MIN_AMP_2 = 0.02;

    RELEASE = 4;
    ATTACK = 2;
    ATTACK_TIMEOUT_MS = 80;
    PEAK_DIFF = 6;

    USE_HANN_WINDOWING = false;
    USE_NUTTALL_WINDOWING = true;    
    
    sampleRate = 16000;
    peakPos = -1;

    AMP_SIZE = (1 + ((WINDOW_SIZE - 1) / 2) + 1);
    window_offset = 512; % this should be different  

    for i=1:window_offset:length(capture_data)
        if (i + WINDOW_SIZE - 1 > length(capture_data))
            break
        end
        %subplot(2,1,1);
        slice = capture_data(i:i + WINDOW_SIZE - 1);
        %plot(slice)
        
        % use nutall windowing
        %subplot(2,1,2);
        for k=1:length(slice)
            new_slice(k) = slice(k)*0.355768 - 0.487396 * ...
                sin(1 * pi * k / WINDOW_SIZE) + 0.144232 * ...
                sin(2 * pi * k / WINDOW_SIZE) - 0.012604 * ... 
                sin(3 * pi * k / WINDOW_SIZE);
        end
        
        %plot(new_slice)
        
        % do FFT analysis
        short_max = 32767;
        raw_samples_norm = new_slice / short_max;
        %n = 2^nextpow2(size(raw_samples_norm,1)); % warum machst du das? das gibt es in der cpp implementation nicht
        n = 1024;
        
        spectrum = fft(raw_samples_norm, n * 2);
        
        amplitudes = [];
        for j=1:AMP_SIZE
            amplitudes(j) = sqrt(real(spectrum(j)) * real(spectrum(j)) + ...
                imag(spectrum(j)) * imag(spectrum(j)) );
        end
        
        % TODO: change minAmp according to an approximated noise level
        currMinAmp = MIN_AMP;
        
        %do peak detection
        min_i = MIN_FREQ * WINDOW_SIZE / sampleRate;
        max_i = MAX_FREQ * WINDOW_SIZE / sampleRate;
        
        lastPeakPos = peakPos;
        peakPos = min_i;
        
        for l=min_i:max_i
            if amplitudes(l) > amplitudes(peakPos)
                peakPos = l;
            end
        end
        
        % check ammplitude of peak and peak to peak distance
        if amplitudes(peakPos) >= currMinAmp && abs(peakPos - lastPeakPos) < PEAK_DIFF
            min_i = peakPos * OVERTONE_MULT_MIN_1;
            max_i = peakPos * OVERTONE_MULT_MAX_1;
            
            peak1Pos = min_i;
            for p=min_i:max_i
                if amplitudes(p) > amplitudes(peak1Pos)
                     peak1Pos = p;
                end
            end
            
            if amplitudes(peak1Pos) >= OVERTONE_MIN_AMP_1
                min_i = peakPos * OVERTONE_MULT_MIN_2;
                max_i = peakPos * OVERTONE_MULT_MAX_2;
                length(amplitudes)
                min_i
                peak2Pos = min_i;                
                for o=min_i:max_i % ist das dasselbe wie <= ?

                     if amplitudes(o) > amplitudes(peak2Pos)
                        peak2Pos = o;
                     end
                end
                
                if amplitudes(peak2Pos) >= OVERTONE_MIN_AMP_2
                    % TODO add the weird attack count from dotmund
                    
                    results = true;
                end

            end
        end    
    end 
    results = false;
end