clear variables
clc

database = 'capture_database.mat';
databasePath = 'D:\Downloads\WhistleData_mat';
capture_path = 'D:\Downloads\WhistleData_mat\capture_recordings';

% adds a raw file to the whistle capture database
try
    load([databasePath '/' database])
     disp('INFO: using a previously created database')
catch
    disp('INFO: no previous mat file was found')
    capture_database = struct;
end


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
    capture = struct;
    capture.game_name = game_name;
    capture.half = half;
    capture.robot_name = robot_name;
    capture.rawData = raw_samples;
    capture.filename = filename;
    capture.samplerate = 8000;
    
    if ~isfield(capture, 'annotations')
        capture.annotations = [];
    end
    showFigs = false;
    if ~isempty(capture.annotations)
        showFigs = true;
    end
    [rate, annot] = readAudacityProjectFile(filepath, capture.rawData, showFigs);
    if rate ~= 0 && ~isempty(annot)
        capture.samplerate = rate;
        capture.annotations = annot;
    end
    clear rate annot
        
    if isfield(capture_database, char(event_name))
        capture_database.(char(event_name)) = [capture_database.(char(event_name)) capture];
    else
        capture_database.(char(event_name)) = capture;
    end
%     clear capture
end

%% Save capture database
disp('Saving Capture database');
save([databasePath '/' database],'capture_database')