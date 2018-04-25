clear variables
clc
%% Example of how to get capture data
database = 'capture_database.mat';
databasePath = 'D:\Downloads\WhistleData_mat';
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
    
    % get only the samples where annotation array is 1
    for k=1:length(list_of_annotated_samples)
        samples = go17_recordings(list_of_annotated_samples(k)).rawData;
        annotations = go17_recordings(list_of_annotated_samples(k)).annotations(:,1);
        
        % sometimes the annotations are longer than the samples
        annotations = annotations(1:length(samples));
        
        % logical indexing
        annotations_idx = annotations == 1;
        whistle_samples = samples(annotations_idx);
        
        % Export samples as wav
        filename = strcat(num2str(k),'.wav');
        Fs = go17_recordings(k).samplerate;
        y = whistle_samples ./ max( abs(whistle_samples(:)) );
        audiowrite(char(filename),y,Fs);
    end
    
    
    
end