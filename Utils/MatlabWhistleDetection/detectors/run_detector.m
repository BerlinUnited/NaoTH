clear all
clc
%% Example of how to get capture data and apply a simple detector
load('L:/Experiments/whistle/WhistleData_mat/capture_database.mat')

%% Get a single capture
go17_recordings = capture_database.go17_recordings;
rc17_recordings = capture_database.rc17_recordings;

% get raw data of first go17 capture
capture_data = go17_recordings(1).rawData;

windows_size = 1024;

results = peak_detector(capture_data, windows_size, 768, 20000);

% Plot results
figure('name', 'peak detector results')
hold on
plot( results.indices, results.values )
for i=1:length(results.indices)
    plot([results.indices(i) results.indices(i) + windows_size - 1], [results.values(i) results.values(i)])
end
hold off

results = zcr_detector(capture_data, 1024, 768, 20000);

% Plot results
figure('name', 'zcr detector results')
hold on
plot( results.indices, results.values )
for i=1:length(results.indices)
    plot([results.indices(i) results.indices(i) + windows_size - 1], [results.values(i) results.values(i)])
end
hold off
