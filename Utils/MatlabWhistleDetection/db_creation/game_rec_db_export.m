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
        disp('INFO: the mat file was found')
        continue;
    end
    
    % TODO export all files and correct folders
end