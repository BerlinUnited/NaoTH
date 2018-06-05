clear all
clc
%% Example of how to get capture data and apply a simple detector
load('L:/Experiments/whistle/WhistleData_mat/capture_database.mat')

load('L:/Experiments/whistle/WhistleData_mat/reference_database.mat')

%% config
windows_size = 1024;

run_zcr = false;
run_peak = false;
run_cross = true;

fftw('planner','estimate');

%% Get a single capture
go17_recordings = capture_database.go17_recordings;
rc17_recordings = capture_database.rc17_recordings;

%% set references whistle set to use
% ref = reference_database.bhuman;
ref = reference_database.kln17;
% ref = reference_database.rc17;


for c = 1:length(go17_recordings) 
    if ~isempty(go17_recordings(c).annotations)
        % get raw data of a go17 capture
        capture_data = go17_recordings(c).rawData;
        record_name = strcat(go17_recordings(c).game_name, '/', go17_recordings(c).half, '/', go17_recordings(c).robot_name, '/', go17_recordings(c).filename);
        %% Max Peak Detector
        if run_zcr
            result = peak_detector(capture_data, windows_size, 768, 20000);
            if true %result.whistle_detected

                % Plot results
                figure('name', strcat("peak detector results on: ", record_name))
                hold on
                plot(result.indices, result.values)
                for i=1:length(result.indices)
                    plot([result.indices(i) result.indices(i) + windows_size - 1], [result.values(i) result.values(i)])
                end
                plot(go17_recordings(c).annotations, 'r')
                plot([1 length(capture_data)], [20000 20000], 'y')
                hold off
            end
        end
        %% ZCR Detector
        if run_peak
            result = zcr_detector(capture_data, 1024, 768, 0.55);
            if true %result.whistle_detected
                % Plot results
                figure('name', strcat("zcr detector results on: ", record_name))
                hold on
                plot(result.indices, result.values)
                for i=1:length(result.indices)
                    plot([result.indices(i) result.indices(i) + windows_size - 1], [result.values(i) result.values(i)])
                end
                plot(go17_recordings(c).annotations, 'r')
                plot([1 length(capture_data)], [0.55 0.55], 'y')
                hold off
            end
        end
        %% Max Cross Correlation Detector       
        if run_cross
            results = maxcrosscorr_detector(capture_data, 1024, 768, 0.25, ref);
             for r = 1:length(results)
                if results(r).whistle_detected
                    % Plot results
                    figure('name', strcat("max cross correlation detector results for: ", results(r).name, " on: ", record_name))
                    hold on
                    plot(results(r).indices, results(r).values)
                    for i=1:length(results(r).indices)
                        plot([results(r).indices(i) results(r).indices(i) + windows_size - 1], [results(r).values(i) results(r).values(i)])
                    end
                    plot(go17_recordings(c).annotations, 'r')
                    plot([1 length(capture_data)], [0.25 0.25], 'y')
                    hold off
                end
             end
        end
    end %if isempty
end %for go17_recordings



