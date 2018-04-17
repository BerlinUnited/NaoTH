clear all
clc
%% Example of how to get capture data and apply a detector
load('../data/capture_database.mat')

%% Get the capture categories
go17_recordings = capture_database.go17_recordings;
rc17_recordings = capture_database.rc17_recordings;

for i=1:length(go17_recordings)
    %TODO export the recordings as wav in the correct folder structure
end

for i=1:length(rc17_recordings)
    %TODO export the recordings as wav in the correct folder structure
end