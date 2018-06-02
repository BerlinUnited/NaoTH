clear variables
clc
%%
database = '2018_03_Rodeo.mat';
databasePath = 'D:\Downloads\WhistleData_mat\game_audio_recordings';
data_path = 'D:\Downloads\WhistleData_mat\game_audio_recordings\2018_03_Rodeo';

% adds a raw file to the whistle capture database
% TODO does not check if file is already inserted
try
    load([databasePath '/' database])
    disp('INFO: using a previously created database')
catch
    disp('INFO: no previous mat file was found')
    game_rec_database = struct;
end

%%

% get information about the location of each raw file inside gamelog_path
folderContents = dir(strcat(data_path, '/**/*.wav'));
%% Extract the necessary infos from filepath
% The following folder structure under data_path is expected:
% data_path (this is ususally the event_folder)
% |-> game_name
%     |-> wav files

eventFolder = split(data_path, ["/","\"]);
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
save([databasePath '/' database],'game_rec_database')