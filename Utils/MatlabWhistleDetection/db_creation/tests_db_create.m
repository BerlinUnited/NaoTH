clear variables
clc

%% init defaults
addpath('../common')
secret()

% adds a raw file to the whistle capture database
try
    load(tests_database_path)
     disp('INFO: using a previously created database')
catch
    disp('INFO: no previous mat file was found')
    test_database = struct;
end


% get information about the location of each raw file inside gamelog_path
folderContents = dir(strcat(test_import_path, '/**/*.raw'));
%% Extract the necessary infos from filepath
% The following folder structure under capture_path is expected:
% capture_path
% |-> event_name
%   |-> game_name
%       |-> half
%           |-> robot_name
%               |-> raw files

eventFolder = split(test_import_path, ["/","\"]);
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
    
    if isfield( test_database, event_name)
        if ~isfield( test_database.(char(event_name))(1), 'annotations')
            dbStuctureChanged = true;
        end
    end
end
if dbStuctureChanged
   test_database = struct;
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

    event_name = string(extractedPath_split(end - 2));
    game_name = string(extractedPath_split(end - 1));
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
        
    if isfield(test_database, char(event_name))
        test_database.(char(event_name)) = [test_database.(char(event_name)) capture];
    else
        test_database.(char(event_name)) = capture;
    end
%     clear capture
end

%% Save capture database
disp('Saving Test database');
save(tests_database_path,'testdatabase')