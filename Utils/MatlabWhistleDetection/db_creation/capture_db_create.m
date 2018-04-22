clear variables
clc
% adds a raw file to the whistle capture database
% TODO does not check if file is already inserted
check_duplicates = false;
try
    load('../data/capture_database.mat')
    disp('INFO: using a previously created database')
    check_duplicates = true;
catch
    disp('INFO: no previous mat file was found')
    capture_database = struct;
end
%%
capture_path = 'D:\Downloads\capture_recordings';

% get information about the location of each raw file inside gamelog_path
folderContents = dir(strcat(capture_path, '/**/*.raw'));
%% Extract the necessary infos from filepath
% The following folder structure under capture_path is expected:
% capture_path
% |-> event_name
%   |-> game_name
%       |-> half
%           |-> robot_name
%               |-> raw files

eventFolder = split(capture_path, ["/","\"]);
eventFolder = eventFolder{end}; % get last folder of gamelog_path

for i=1:length(folderContents)
    % use the path of the first raw file
    testfolder = folderContents(i).folder;
    extractedPath = extractAfter(testfolder,eventFolder);
    
    % remove slash in front
    if strcmp(extractedPath(1), '\') || strcmp(extractedPath(1), '/')
        extractedPath = extractedPath(2:end);
    end
    
    extractedPath_split = split(extractedPath, ["/","\"]);

    event_name = string(extractedPath_split(1));
    game_name = string(extractedPath_split(2));
    half = string(extractedPath_split(3));
    robot_name = string(extractedPath_split(4));
    filename = folderContents(i).name;
    
    % get raw file
    filepath = fullfile(folderContents(i).folder, folderContents(i).name);
    % read raw data from file
    fileID = fopen (filepath, 'r');
        raw_samples = fread(fileID, 'int16', 'ieee-le');
    fclose(fileID);
    
    % fill the capture whistle fields
    capture.game_name = game_name;
    capture.half = half;
    capture.robot_name = robot_name;
    capture.rawData = raw_samples;
    capture.filename = filename;
    capture.samplerate = 8000; 
    if isfield(capture_database, event_name)
        % Check if this capture already exists
        if(check_duplicates)
            % TODO iterate over existing db
        end
        
        % append a capture to the event struct
        capture_database.(event_name) = [capture_database.(event_name) capture];
    else
        capture_database.(event_name) = capture;
    end
end

%% Save capture database
disp('Saving Capture database');
save('../data/capture_database.mat','capture_database')