clear variables
clc
%% Example of how to get capture data and apply a detector
load('../data/capture_database.mat')


%% Get the capture categories
go17_recordings = capture_database.go17_recordings;
rc17_recordings = capture_database.rc17_recordings;

for i=1:length(go17_recordings)
    % Export the recordings as raw in the correct folder structure
    game_name = go17_recordings(i).game_name;
    half = go17_recordings(i).half;
    robot_name = go17_recordings(i).robot_name;
    rawData = go17_recordings(i).rawData;
    
    path_to_outputfolder = fullfile('../data/capture_data_raw/go17_recordings', game_name, half, robot_name);
    mkdir(path_to_outputfolder)
    
    % TODO raw files should have the original filename, if database creation is
    % changed fix this
    filename = fullfile(path_to_outputfolder, strcat(int2str(i), '.raw'));
    
    fileID = fopen(filename, 'w');
        fwrite(fileID, rawData, 'int16', 'ieee-le');
    fclose(fileID);
    
    % Export samples as wav
    % TODO wav files should have the original filename, if database creation is
    % changed fix this
    filename2 = fullfile(path_to_outputfolder, strcat(int2str(i), '.wav'));
    Fs=8000;
    y = rawData ./ max( abs(rawData(:)) );
    audiowrite(char(filename2),y,Fs);
end

for i=1:length(rc17_recordings)
    % Export the recordings as raw in the correct folder structure
    game_name = rc17_recordings(i).game_name;
    half = rc17_recordings(i).half;
    robot_name = rc17_recordings(i).robot_name;
    rawData = rc17_recordings(i).rawData;
    
    path_to_outputfolder = fullfile('../data/capture_data_raw/rc17_recordings', game_name, half, robot_name);
    mkdir(path_to_outputfolder)
    
    % TODO raw files should have the original filename, if database creation is
    % changed fix this
    filename = fullfile(path_to_outputfolder, strcat(int2str(i), '.raw'));
    
    fileID = fopen (filename, 'w');
        fwrite(fileID, rawData, 'int16', 'ieee-le');
    fclose(fileID);
    
    % Export samples as wav
    % TODO wav files should have the original filename, if database creation is
    % changed fix this
    filename2 = fullfile(path_to_outputfolder, strcat(int2str(i), '.wav'));
    Fs=8000;
    y = rawData ./ max( abs(rawData(:)) );
    audiowrite(char(filename2),y,Fs);
end