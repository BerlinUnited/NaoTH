clear variables
clc
%% init defaults
addpath('../common')
secret()

% adds a raw file to the whistle reference database
% TODO does not check if file is already inserted
try
    load(reference_database_path)
    disp('INFO: using a previously created database')
catch
    disp('INFO: no previous mat file was found')
    reference_database = struct;
end
%%
% get subfolders that hold the audio recordings from different events
d = dir(reference_import_path);
isub = [d(:).isdir]; %# returns logical vector
categorie_names = {d(isub).name}';
%remove . and ..
categorie_names(ismember(categorie_names,{'.','..'})) = [];

%% iterate over each reference categorie
for m=1:length(categorie_names)
    
    folderblabla = strcat(reference_import_path, '\', categorie_names(m));
    folderContents = dir(strcat(char(folderblabla), '/**/*.raw'));
    
    for d = 1:length(folderContents)
        
        reference_database = add_whistle(reference_database, ...
        [folderContents(d).folder '/' folderContents(d).name], 1, 8000, cell2mat(categorie_names(m)));
    end
end
%%
disp('Saving Whistle database');
save(reference_database_path, 'reference_database','-v7.3')