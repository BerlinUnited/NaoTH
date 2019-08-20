clear variables
clc
%% init defaults
addpath('../common')
secret()

try
    load(capture_database_path)
    disp('Loading the capture database')
catch
    disp('ERROR: No capture database was found')
end
%Test
exportPath = '.';

%% Get the capture categories
events = fieldnames(capture_database);
for i=1:length(events)
    current_event = char(events(i));
    event_struct = capture_database.(current_event);
    
    list_of_annotated_samples = [];
    for j=1:length(event_struct)
        % check if annotations exists
        if isfield(event_struct, 'annotations')
            if ~isempty(event_struct(j).annotations)
                list_of_annotated_samples = [list_of_annotated_samples j];
            end 
        end
    end
    
    %
    for k=1:length(list_of_annotated_samples)

        % BUG assumes that only on whistle per recording
        samples = event_struct(list_of_annotated_samples(k)).rawData;
        annotations = event_struct(list_of_annotated_samples(k)).annotations(:,1);

        % sometimes the annotations are longer than the samples
        annotations = annotations(1:length(samples));
        
        % test if something is labeled, continue otherwise
        if sum(annotations) == 0
            continue
        end
        
        % logical indexing
        annotations_idx = annotations == 1;
        whistle_samples = samples(annotations_idx);

        %%
        game_name = event_struct(k).game_name;
        half = event_struct(k).half;
        robot_name = event_struct(k).robot_name;
        filename = event_struct(k).filename;

        % Export full whistle samples as wav
        %TODO better name
        path_to_outputfolder = fullfile(exportPath, current_event, game_name, half, robot_name);
        mkdir(path_to_outputfolder);
        export_filename = fullfile(path_to_outputfolder, strcat(filename, '-fullwhistle', '.wav'));

        Fs = event_struct(k).samplerate;
        y = whistle_samples ./ max( abs(whistle_samples(:)) );
        audiowrite(char(export_filename),y,Fs);

        % Export references
        % use 1024 samples from whistle_samples
        ueberlaenge = length(whistle_samples) - 1024;
        ueberlaenge_front = floor(ueberlaenge / 2);

        reference_samples = whistle_samples(length(ueberlaenge_front):length(ueberlaenge_front) + 1024 -1);

        path_to_outputfolder = fullfile(exportPath, current_event, game_name, half, robot_name);
        mkdir(path_to_outputfolder);

        % Export new reference whistle as wav
        export_filename = fullfile(path_to_outputfolder, strcat(filename, '-referencewhistle', '.wav'));
        Fs = event_struct(k).samplerate;
        y = reference_samples ./ max( abs(reference_samples(:)) );
        audiowrite(char(export_filename),y,Fs);
    end
    
end