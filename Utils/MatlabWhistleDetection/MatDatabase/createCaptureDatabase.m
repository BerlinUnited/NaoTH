clear all
clc
% adds a raw file to the whistle capture database
% TODO does not check if file is already inserted
try
    load('capture_database.mat')
    disp('INFO: using a previously created database')
catch
    disp('INFO: no previous mat file was found')
    capture_database = struct;
end
%%
gamelog_path = '../data/rc17_recordings'; % TODO rename gamelog_path

% get information about the location of each raw file inside gamelog_path
folderContents = dir(strcat(gamelog_path, '/**/*.raw'));
%% Extract the necessary infos from filepath
% The following folder structure under gamelog_path is expected:
% gamelog_path
% |-> game_name
%     |-> half
%         |-> robot_name
%             |-> raw files

eventFolder = split(gamelog_path, ["/","\"]);
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

    game_name = string(extractedPath_split(1));
    half = string(extractedPath_split(2));
    robot_name = string(extractedPath_split(3));
    
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
    if isfield(capture_database, eventFolder) 
        capture_database.(eventFolder) = [capture_database.(eventFolder) capture];
    else
        capture_database.(eventFolder) = capture;
    end
end

%% Save capture database
disp('Saving Capture database');
save('capture_database.mat','capture_database')