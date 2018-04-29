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

% % TODO the paths to the raw reference files should be collected differently

% go 17 whistles
folderContents = dir(strcat(reference_import_path, 'go17/', '/**/*.dat'));
for d = 1:length(folderContents)
    reference_database = add_whistle_spectrum(reference_database, [folderContents(d).folder '/' folderContents(d).name], 1, 8000, 'go17');
end

% % go 18 whistles
folderContents = dir(strcat(reference_import_path, 'go18/', '/**/*.dat'));
for d = 1:length(folderContents)
    reference_database = add_whistle_spectrum(reference_database, [folderContents(d).folder '/' folderContents(d).name], 1, 8000, 'go18');
end

% bhuman reference whistles
folderContents = dir(strcat(reference_import_path, 'bhuman_references/', '/**/*.dat'));
for d = 1:length(folderContents)
    reference_database = add_whistle(reference_database, [folderContents(d).folder '/' folderContents(d).name], 1, 8000, 'bhuman');
end

% kln17 reference whistles
folderContents = dir(strcat(reference_import_path, 'kln17_references/', '/**/*.dat'));
for d = 1:length(folderContents)
    reference_database = add_whistle(reference_database, [folderContents(d).folder '/' folderContents(d).name], 1, 8000, 'kln17');
end

folderContents = dir(strcat(reference_import_path, 'kln17_references/', '/**/*.dat'));
for d = 1:length(folderContents)
    reference_database = add_whistle(reference_database, [folderContents(d).folder '/' folderContents(d).name], 1, 8000, 'kln17');
end

% rc17 reference whistles
folderContents = dir(strcat(reference_import_path, 'rc17_references/', '/**/*.dat'));
for d = 1:length(folderContents)
    reference_database = add_whistle(reference_database, [folderContents(d).folder '/' folderContents(d).name], 1, 8000, 'rc17');
end

disp('Saving Whistle database');
save(reference_database_path, 'reference_database')