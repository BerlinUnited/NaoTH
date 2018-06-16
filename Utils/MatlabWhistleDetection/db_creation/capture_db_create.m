clear variables
clc

%% init defaults
addpath('../common')
secret()

% adds a raw file to the whistle capture database
try
    load(capture_database_path)
    disp('INFO: using a previously created database')
catch
    disp('INFO: no previous mat file was found')
    capture_database = struct;
end


% get information about the location of each raw file inside gamelog_path
folderContents = dir(strcat(capture_import_path, '/**/*.raw'));
%% Extract the necessary infos from filepath
% The following folder structure under capture_import_path is expected:
% capture_import_path
% |-> event_name
%   |-> game_name
%       |-> half
%           |-> robot_name
%               |-> raw files

eventFolder = split(capture_import_path, ["/","\"]);
eventFolder = eventFolder{end}; % get last folder of gamelog_path

%%
dbStuctureChanged = false;
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
    
    if isfield( capture_database, event_name)
        if ~isfield( capture_database.(char(event_name))(1), 'annotations')
            dbStuctureChanged = true;
        end
    end
end
if dbStuctureChanged
   capture_database = struct;
   disp('INFO: database structure changed creating new database')
end

%%
for i=1:length(folderContents)
    % use the path of the first raw file
    testfolder = folderContents(i).folder;
    extractedPath = extractAfter(testfolder,eventFolder);
    
    % remove slash in front
    if strcmp(extractedPath(1), '\') || strcmp(extractedPath(1), '/')
        extractedPath = extractedPath(2:end);
    end
    
    extractedPath_split = split(extractedPath, ["/","\"]);

    event_name = string(extractedPath_split(end - 3));
    game_name = string(extractedPath_split(end - 2));
    half = string(extractedPath_split(end - 1));
    robot_name = string(extractedPath_split(end));
    filename = folderContents(i).name;
    
    % get raw file
    filepath = fullfile(folderContents(i).folder, folderContents(i).name);
    % read raw data from file
    fileID = fopen (filepath, 'r');
        raw_samples = fread(fileID, 'int16', 'ieee-le');
    fclose(fileID);
    
    % fill the capture whistle fields
    capture = struct;
    capture.game_name = game_name;
    capture.half = half;
    capture.robot_name = robot_name;
    capture.rawData = raw_samples;
    capture.filename = filename;
    capture.samplerate = 8000;
    capture.annotations = [];
    [rate, annot] = readAudacityProjectFile(filepath, capture.rawData, false);
    if rate ~= 0 && ~isempty(annot)
        capture.samplerate = rate;
        capture.annotations = annot;
    end
    clear rate annot
    
    % checks if an event struct exists in the database
    if isfield(capture_database, char(event_name))
        % check if entry is already there via rawData comparision
        % -> disabled because it takes too long
%         event_struct = capture_database.(char(event_name));
%         for j=1:length(event_struct)
%             test_raw_data = event_struct(j).rawData;
%             if(isequal(test_raw_data, capture.rawData))
%                 break
%             else
%                 % append capture to event struct
%                 capture_database.(char(event_name)) = [capture_database.(char(event_name)) capture];
%             end
%         end
        % append capture to event struct
        capture_database.(char(event_name)) = [capture_database.(char(event_name)) capture];
    else
        % create new event struct
        capture_database.(char(event_name)) = capture;
    end

end

%% Save capture database
disp('Saving Capture database');
save(capture_database_path, 'capture_database','-v7.3')