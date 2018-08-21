clear variables
clc
%% init defaults
addpath('../common')
secret()

% get subfolders that hold the audio recordings from different events
d = dir(game_import_path);
isub = [d(:).isdir]; %# returns logical vector
event_names = {d(isub).name}';
%remove . and ..
event_names(ismember(event_names,{'.','..'})) = [];

%% iterate over each event
for m=1:length(event_names)
    
    % check if a database file exists with the same name
    event_db_path = strcat(game_database_path,'\',event_names(m),'.mat');
    
    try
        load(cell2mat(event_db_path))
        fprintf('INFO: using a previously created database from\n%s\n', cell2mat(event_db_path))
    catch
        disp('INFO: no previous mat file was found')
        game_rec_database = struct;
    end
    
    event_import_path = cell2mat(strcat(game_import_path, '\',event_names(m)));
    % get information about the location of each raw file inside gamelog_path
    folderContents = dir(strcat(event_import_path, '/**/*.wav'));
    %% Extract the necessary infos from filepath
    % The following folder structure under data_path is expected:
    % event_import_path
    % |-> game_name
    %     |-> wav files

    eventFolder = split(event_import_path, ["/","\"]);
    eventFolder = eventFolder{end}; % get last folder of data_path
    
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

        % get wav file
        filename = string(folderContents(i).name);
        filepath = fullfile(folderContents(i).folder, folderContents(i).name);
        [y,Fs] = audioread(filepath);

        % sometimes the exported wav file has two channels that are the same
        if(size(y,2) == 2)
            y = y(:,1);
        end

        % subsample it so that it has the same frequency as the robot(8kHz)
        target_samplerrate = 8000;
        [P,Q] = rat(target_samplerrate / Fs);
        y = resample(y,P,Q);

        % fill the capture whistle fields
        recording.game_name = game_name;
        recording.file_name = filename;
        recording.samples = y;
        recording.sample_rate = target_samplerrate;

        % create name for event that follows the struct field naming convention
        eventname = ['e_',eventFolder];
        if isfield(game_rec_database, eventname) 
            game_rec_database.(eventname) = [game_rec_database.(eventname) recording];
        else
            game_rec_database.(eventname) = recording;
        end

    end
    
    %% Save capture database
    disp('Saving Game Recording database');
    save(cell2mat(event_db_path),'game_rec_database','-v7.3')
end