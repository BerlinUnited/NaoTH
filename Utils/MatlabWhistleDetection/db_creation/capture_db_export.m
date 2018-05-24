clear variables
clc
%% init defaults
warning('off', 'MATLAB:MKDIR:DirectoryExists');
addpath('../common')
secret()

try
    load(capture_database_path)
    disp('Loading the capture database')
catch
    disp('ERROR: No capture database was found')
    return;
end

%% Get the capture categories
events = fieldnames(capture_database);
for k=1:length(events)
    current_event = char(events(k));
    event_struct = capture_database.(current_event);
    for i=1:length(event_struct)
        % Export the recordings as raw in the correct folder structure
        game_name = event_struct(i).game_name;
        half = event_struct(i).half;
        robot_name = event_struct(i).robot_name;
        rawData = event_struct(i).rawData;
        filename = event_struct(i).filename;

        path_to_outputfolder = fullfile(capture_export_path, current_event, game_name, half, robot_name);
        mkdir(path_to_outputfolder);

        fileID = fopen(fullfile(path_to_outputfolder, filename), 'w');
            fwrite(fileID, rawData, 'int16', 'ieee-le');
        fclose(fileID);

        % Export samples as wav
        [~,filepartname,~] = fileparts(filename);
        filename2 = fullfile(path_to_outputfolder, strcat(filepartname, '.wav'));
        Fs = event_struct(i).samplerate;
        y = rawData ./ max( abs(rawData(:)) );
        audiowrite(char(filename2),y,Fs);
    end
end