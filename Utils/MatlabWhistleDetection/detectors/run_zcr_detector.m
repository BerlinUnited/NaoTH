clear variables
clc
%% init defaults
addpath('../common')
secret()

databases = struct;
% load databases
try
    load(capture_database_path)
catch
    disp('ERROR: Could not load capture database')
    return
end

try
    load(reference_database_path)
catch
    disp('ERROR: Could not load reference database')
    return
end

% try
%     load(tests_database_path)
% catch
%     disp('ERROR: Could not load test database')
%     return
% end

%% config
config = {};
config.window_size = 1024;
config.window_offset = 786;

fftw('planner','estimate');

database = capture_database;

create_detection_plots = false;
create_non_detection_plots = false;

%% set references whistle set to use
% ref = reference_database.bhuman;
% ref = reference_database.kln17;
% ref = reference_database.rc17;
% ref = reference_database.go18;
ref = reference_database.go17;

ref_stat = struct;
for r = 1:length(ref)
    % replace points and minus signs with underscore
    ref(r).name = strrep(ref(r).name, '-', '_');
    ref(r).name = strrep(ref(r).name, '.', '_');
    
    ev  = struct;
    ev.tp = 0;
    ev.fp = 0;
    ref_stat.(char(ref(r).name)) = ev;
end
%%

recordings_names = fieldnames(database);
for t = 1:length(recordings_names)
    recording_name = recordings_names(t);
    recording = database.(recording_name{1});
    for c = 1:length(recording)
        if ~isempty(recording(c).annotations)
            % get raw data
            capture_data = recording(c).rawData;
            record_name = strcat(recording(c).game_name, " ", recording(c).half, " ", recording(c).robot_name, " ", recording(c).filename);
            %% ZCR Detector
            result = zcr_detector(capture_data, config.window_size, config.window_offset, 0.55);
            if result.whistle_detected
                % Plot results
                figure('name', strcat("zcr detector results on: ", record_name))
                hold on
                plot(result.indices, result.values)
                for i=1:length(result.indices)
                    plot([result.indices(i) result.indices(i) + config.window_size - 1], [result.values(i) result.values(i)])
                end
                plot(recording(c).annotations, 'r')
                plot([1 length(capture_data)], [0.55 0.55], 'y')
                hold off
            end
        end %if ~isempty(recording(c).annotations)
    end %c = 1:length(recording)    
end %for t = 1:length(capture_database)