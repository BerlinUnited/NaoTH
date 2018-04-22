clear variables
clc
%% Example of how to get capture data and apply a detector
load('../data/reference_database.mat')

%% Get the reference categories
ref_bhuman = reference_database.bhuman;
ref_kln17= reference_database.kln17;
ref_rc17= reference_database.rc17;

for i=1:length(ref_bhuman)    
    % Export the recordings as raw in the correct folder structure
    name = ref_bhuman(i).name;
    rawData = ref_bhuman(i).rawData;
    
    path_to_outputfolder = fullfile('../data/reference_data_raw/bhuman');
    mkdir(path_to_outputfolder)
    
    filename = fullfile(path_to_outputfolder, strcat(name, '.raw'));
    
    % Export raw data
    fileID = fopen(filename, 'w');
        fwrite(fileID, rawData, 'int16', 'ieee-le');
    fclose(fileID);
    
    % TODO Export wav
    
    %TODO export the spectrum as dat in the correct folder structure 
    %for spectrum export the exportSpectrum.m script can be used
end

for i=1:length(ref_kln17)
    % Export the recordings as raw in the correct folder structure
    name = ref_kln17(i).name;
    rawData = ref_kln17(i).rawData;
    
    path_to_outputfolder = fullfile('../data/reference_data_raw/kln17');
    mkdir(path_to_outputfolder)
    
    filename = fullfile(path_to_outputfolder, strcat(name, '.raw'));
    
    % Export raw data
    fileID = fopen(filename, 'w');
        fwrite(fileID, rawData, 'int16', 'ieee-le');
    fclose(fileID);
    
    % TODO Export wav
    
    %TODO export the spectrum as dat in the correct folder structure 
    %for spectrum export the exportSpectrum.m script can be used
end

for i=1:length(ref_rc17)
    % Export the recordings as raw in the correct folder structure
    name = ref_rc17(i).name;
    rawData = ref_rc17(i).rawData;
    
    path_to_outputfolder = fullfile('../data/reference_data_raw/rc17');
    mkdir(path_to_outputfolder)
    
    filename = fullfile(path_to_outputfolder, strcat(name, '.raw'));
    
    % Export raw data
    fileID = fopen(filename, 'w');
        fwrite(fileID, rawData, 'int16', 'ieee-le');
    fclose(fileID);
    
    % TODO Export wav
    
    %TODO export the spectrum as dat in the correct folder structure 
    %for spectrum export the exportSpectrum.m script can be used
end