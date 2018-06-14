clear variables
clc
%% Export Capture data
secret() % import the custom paths

load(capture_database_path)

%% Get the capture categories
go17_recordings = capture_database.go17_recordings;
rc17_recordings = capture_database.rc17_recordings;

for i=1:length(go17_recordings)
    % Export the recordings as raw in the correct folder structure
    game_name = go17_recordings(i).game_name;
    half = go17_recordings(i).half;
    robot_name = go17_recordings(i).robot_name;
    rawData = go17_recordings(i).rawData;
    filename = go17_recordings(i).filename;
    
    path_to_outputfolder = fullfile(capture_export_path, 'go17_recordings', game_name, half, robot_name);
    mkdir(path_to_outputfolder)
    
    fileID = fopen(fullfile(path_to_outputfolder, filename), 'w');
        fwrite(fileID, rawData, 'int16', 'ieee-le');
    fclose(fileID);
    
    % Export samples as wav
    [~,filepartname,~] = fileparts(filename);
    filename2 = fullfile(path_to_outputfolder, strcat(filepartname, '.wav'));
    Fs = go17_recordings(i).samplerate;
    y = rawData ./ max( abs(rawData(:)) );
    audiowrite(char(filename2),y,Fs);
end

%%
for i=1:length(rc17_recordings)
    % Export the recordings as raw in the correct folder structure
    game_name = rc17_recordings(i).game_name;
    half = rc17_recordings(i).half;
    robot_name = rc17_recordings(i).robot_name;
    rawData = rc17_recordings(i).rawData;
    filename = rc17_recordings(i).filename;
    
    %TODO change the path
    path_to_outputfolder = fullfile(exportPath, 'rc17_recordings', game_name, half, robot_name);
    mkdir(path_to_outputfolder)
    
    fileID = fopen (fullfile(path_to_outputfolder, filename), 'w');
        fwrite(fileID, rawData, 'int16', 'ieee-le');
    fclose(fileID);
    
    % Export samples as wav
    [~,filepartname,~] = fileparts(filename);
    filename2 = fullfile(path_to_outputfolder, strcat(filepartname, '.wav'));
    Fs = rc17_recordings(i).samplerate;
    y = rawData ./ max( abs(rawData(:)) );
    audiowrite(char(filename2),y,Fs);
end