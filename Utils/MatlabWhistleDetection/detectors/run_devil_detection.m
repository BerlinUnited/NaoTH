clear variables
clc
%% init defaults
addpath('../common')
secret()

% load databases
try
    load(capture_database_path)
    disp('Loaded capture database')
catch
    disp('ERROR: Could not load capture database')
    return
end

%%Settings
event_names = fieldnames(capture_database);
% Check first event only for now
event = capture_database.(event_names{3});

%% TODO this only tests files that have a positive annotation
% TODO files that have no whistles currently have no annotations
annotated_capture = [];
for capture_idx = 1:length(event)
    if ~isempty(event(capture_idx).annotations)
        annotated_capture(end+1) = capture_idx;
    end
end

%%
%HACK
annotated_capture = [3];
for k=1:length(annotated_capture)
        % get raw capture data
        capture_data = event(annotated_capture(k)).rawData;
        event(annotated_capture(k)).filename
        % Todo resample to same frequency the nao devils use
        new_data = resample(capture_data, 20500, 48000);
        
        %spectrogram(new_data,128,120,128,20500,'yaxis')
        
        %annotations = event(annotated_capture(k)).annotations(:,1);
        detected_whistles = zeros(length(new_data),1);
        detected_peaks = zeros(length(new_data),1);
        peak_maxima = zeros(length(new_data),1);
        % test for detection
        % parameters from nao devils
        WINDOW_SIZE = 1024;

        MIN_FREQ = 2500;
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

        sampleRate = 20500;
        peakPos = -1;
        
        % possible bug
        %AMP_SIZE = (1 + ((WINDOW_SIZE - 1) / 2) + 1);
        AMP_SIZE = 1024;
        
        window_offset = 768; % this should be different  
        lastAttackTime = 0;
        whistleCount = 0;
        
        for i=1:window_offset:length(new_data)
            if (i + WINDOW_SIZE - 1 > length(new_data))
                break
            end
            
            % slice ist das fenster
            slice = new_data(i:i + WINDOW_SIZE - 1);
            
            % normalize the samples between 0 and short_max
            short_max = 32767;
            raw_samples_norm = slice / short_max;
            n = 1024;
            
            % use nutall windowing
            for q=1:length(slice)
                new_slice(q) = slice(q)*0.355768 - 0.487396 * ...
                    sin(1 * pi * q / WINDOW_SIZE) + 0.144232 * ...
                    sin(2 * pi * q / WINDOW_SIZE) - 0.012604 * ... 
                    sin(3 * pi * q / WINDOW_SIZE);
            end  
            
            % do FFT analysis
            spectrum = fft(raw_samples_norm, n * 2);
           
            amplitudes = abs(spectrum);
            amplitudes = amplitudes(1:int32(AMP_SIZE));
            %plot(amplitudes)
            %hold on

            %do peak detection
            % time resolution = WINDOW_SIZE / sampleRate
            min_index = int32(MIN_FREQ * WINDOW_SIZE / sampleRate);
            max_index = int32(MAX_FREQ * WINDOW_SIZE / sampleRate);
            
            min_index = min_index * 2;
            max_index = max_index * 2;
            lastPeakPos = peakPos;
            peakPos = min_index;
    
            for l=min_index:max_index
                if amplitudes(l) > amplitudes(peakPos)
                    peakPos = l;
                    
                end
            end
            peak_maxima(i:i+WINDOW_SIZE) = peakPos;
            plot(amplitudes)
            hold on
            plot([min_index min_index], [0 100]);
            plot([max_index max_index], [0 100]);
            hold off
            %plot(peakPos, amplitudes(peakPos), 'o')
            %hold on
            
            
            % check ammplitude of peak and peak to peak distance
            if amplitudes(peakPos) >= MIN_AMP
                detected_peaks(i:i+WINDOW_SIZE) = 1; % Debug
                if abs(peakPos - lastPeakPos) < PEAK_DIFF
                    min_i = int32(peakPos * OVERTONE_MULT_MIN_1);
                    max_i = int32(peakPos * OVERTONE_MULT_MAX_1);

                    

                    peak1Pos = min_i;
                    for p=min_i:max_i
                        if amplitudes(p) > amplitudes(peak1Pos)
                             peak1Pos = p;
                        end
                    end
                    %plot(peak1Pos, amplitudes(peak1Pos), 'o')
                    %hold on

                    if amplitudes(peak1Pos) >= OVERTONE_MIN_AMP_1
                        min_i = peakPos * OVERTONE_MULT_MIN_2;
                        max_i = peakPos * OVERTONE_MULT_MAX_2;

                        peak2Pos = min_i;                
                        for o=min_i:max_i
                             if amplitudes(o) > amplitudes(peak2Pos)
                                peak2Pos = o;
                             end
                        end
                        %plot(peak2Pos, amplitudes(peak2Pos), 'o')


                        if amplitudes(peak2Pos) >= OVERTONE_MIN_AMP_2

                            if lastAttackTime - i < (ATTACK_TIMEOUT_MS / 1000) * 48000
                                whistleCount = whistleCount +1;

                                if whistleCount >= 1
                                    results = true;
                                    window_start = i;
                                    detected_whistles(i:i+WINDOW_SIZE) = 1;
                                end
                            else
                                whistleCount = 0;
                            end
                            lastAttackTime = i;
                        else
                            %disp('Amplitude of peak2 to low')
                        end
                    else
                        %disp('Amplitude of peak1 to low')
                    end
                else
                    %disp('peak is to far away')
                    %clf;
                end %peak diff
            end % peak amplitude is to low
            %return
        end
        
        clf;
        
        
        %TODO: make plot of whole capture 
         % -> which things should be drawn there
        subplot(3,1,1);
        plot(new_data/max(new_data), 'b')
        %hold on
        %plot(annotations, 'g')
        hold on
        plot(detected_whistles, 'r')
        subplot(3,1,2);
        plot(new_data/max(new_data), 'b')
        hold on
        plot(detected_peaks, 'y')
        subplot(3,1,3);
        plot(peak_maxima, 'b')
        
        
        % todo calculate stuff for each window -> plot it if there was a
        % detection
end