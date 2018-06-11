clear variables
clc
%% init defaults
warning('off', 'MATLAB:MKDIR:DirectoryExists');
addpath('../common')
secret()

% load databases
try
    load(capture_database_path)
catch
    disp('ERROR: Could not load capture database')
    return
end
%% Find all annotated captures containing whistles
event_names = fieldnames(capture_database);
% container for references
annotated_captures = struct; % those are not extracted yet -> choose better name

for t = 1:length(event_names)
    event_name = event_names(t);
    event_recordings = capture_database.(event_name{1});
    
    %sprintf("Use captures from %s", cell2mat(event_name))
    
    for c = 1:length(event_recordings)
        if ~isempty(event_recordings(c).annotations)
            % append row to extracted_references
            an = event_recordings(c);
            
            if isfield(annotated_captures, char(event_name))
                annotated_captures.(char(event_name)) = [annotated_captures.(char(event_name)) an];
            else   
                annotated_captures.(char(event_name)) = an;
            end
        end
    end    
end

%% Get the first 1024 samples of positive annotated samples in each capture
extracted_references = annotated_captures;
event_names = fieldnames(extracted_references);
for t = 1:length(event_names)
    event_name = event_names(t);
    event_recordings = capture_database.(event_name{1});

    for c = 1:length(event_recordings)        
        an = event_recordings(c);
        if ~isempty(an.annotations)
            % assumes that max one whistle is marked per capture
            whistle_marker = an.annotations(:,1);

            whistleData = an.rawData(logical(whistle_marker));

            % shorten the whistledata to 1024 samples
            if length(whistleData) < 1024
                continue
            else
                whistleData = whistleData(1:1024);
            end        

            % export whistle data as raw
            path_to_outputfolder = fullfile(reference_export_path,strcat('gen_from_', char(event_name)));
            mkdir(path_to_outputfolder);
            
            
            record_name = strcat(an.game_name, "_", an.half, "_", an.robot_name,"_", int2str(c), ".raw");
            
            % make sure that the filename does not start with a number that
            % fixes later problems in the analysis scripts
            a = char(record_name);
            a = a(1); % get first character
            a = str2num(a); % try the convert it to a number
            if a
                record_name = strcat('g', record_name)
            end
            
            fileID = fopen(fullfile(path_to_outputfolder, record_name), 'w');
                fwrite(fileID, whistleData, 'int16', 'ieee-le');
            fclose(fileID);

            %TODO export whistle data as wav
        end
    end
end