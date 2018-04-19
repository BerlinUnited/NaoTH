clear all
clc
%% Example of how to get capture data and apply a simple detector
load('../data/capture_database.mat')

%% Get a single capture
go17_recordings = capture_database.go17_recordings;
rc17_recordings = capture_database.rc17_recordings;

% get raw data of first go17 capture
capture_data = go17_recordings(1).rawData;

%% Run Peak Detection on a single capture
peak_threshold = 20000;
for i=1:512:length(capture_data)
    if (i+1024-1 > length(capture_data))
        break
    end
    slice = capture_data(i:i+1024-1);
    max_slice = max(slice);
    
    % TODO write statistik in capture database
    if(max_slice > peak_threshold)
        disp('Whistle detected')
    end
    % Plot Peak level in every slice
    plot( [i i+1024-1], [max_slice max_slice] )
    hold on
end
hold off

