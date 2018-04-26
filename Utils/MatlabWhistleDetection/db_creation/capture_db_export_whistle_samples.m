clear variables
clc
%% Example of how to get capture data
database = 'capture_database.mat';
databasePath = 'D:\Downloads\WhistleData_mat';
exportPath = fullfile(databasePath, 'capture_recordings');
load([databasePath '/' database])

%% Get the capture categories
go17_recordings = capture_database.go17_recordings;
rc17_recordings = capture_database.rc17_recordings;

list_of_annotated_samples = [];
for j=1:length(go17_recordings)
    % check if annotations exists
    if isfield(go17_recordings, 'annotations')
        if ~isempty(go17_recordings(j).annotations)
            list_of_annotated_samples = [list_of_annotated_samples j];
        end 
    end
end

% get only the samples where annotation array is 1
for k=1:length(list_of_annotated_samples)
    
    % BUG assumes that only on whistle per recording
    samples = go17_recordings(list_of_annotated_samples(k)).rawData;
    annotations = go17_recordings(list_of_annotated_samples(k)).annotations(:,1);

    % sometimes the annotations are longer than the samples
    annotations = annotations(1:length(samples));

    % logical indexing
    annotations_idx = annotations == 1;
    whistle_samples = samples(annotations_idx);
    
    %%
    game_name = go17_recordings(k).game_name;
    half = go17_recordings(k).half;
    robot_name = go17_recordings(k).robot_name;
    filename = go17_recordings(k).filename;
    
    % Export full whistle samples as wav
    %TODO better name
    path_to_outputfolder = fullfile(exportPath, 'go17_recordings', game_name, half, robot_name);
    mkdir(path_to_outputfolder);
    export_filename = fullfile(path_to_outputfolder, strcat(filename, '-fullwhistle', '.wav'));
    
    Fs = go17_recordings(k).samplerate;
    y = whistle_samples ./ max( abs(whistle_samples(:)) );
    audiowrite(char(export_filename),y,Fs);

    % Export references
    % use 1024 samples from whistle_samples
    ueberlaenge = length(whistle_samples) - 1024;
    ueberlaenge_front = floor(ueberlaenge / 2);

    reference_samples = whistle_samples(length(ueberlaenge_front):length(ueberlaenge_front) + 1024 -1);
    
    path_to_outputfolder = fullfile(exportPath, 'go17_recordings', game_name, half, robot_name);
    mkdir(path_to_outputfolder);
    
    % Export new reference whistle as wav
    export_filename = fullfile(path_to_outputfolder, strcat(filename, '-referencewhistle', '.wav'));
    Fs = go17_recordings(k).samplerate;
    y = reference_samples ./ max( abs(reference_samples(:)) );
    audiowrite(char(export_filename),y,Fs);
end
    
    
    
