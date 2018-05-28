clear variables
clc
%% init defaults
addpath('../common')
secret()

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

run_zcr = false;
run_peak = true;
run_cross = false;

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
event_names = fieldnames(database);
for t = 1:length(event_names)
    event_name = event_names(t);
    event = database.(event_name{1});
    %TODO: output the event that is being checked
    for c = 1:length(event)
        sprintf("Check file %d of %d",c, length(event))
        if ~isempty(event(c).annotations)
            sprintf("Found Annotations for %d", c)
            % Max Peak Detector
            threshold = 20000;
            record_name = strcat(event(c).game_name, " ", event(c).half, " ", event(c).robot_name, " ", event(c).filename);
            result = peak_detector(event(c).rawData, config, threshold);

            if result.whistle_detected
                % Plot results
                figure('name', strcat("peak detector results on: ", record_name))
                hold on
                plot(result.indices, result.values)
                for i=1:length(result.indices)
                    plot([result.indices(i) result.indices(i) + config.window_size - 1], [result.values(i) result.values(i)])
                end
                plot(event(c).annotations, 'r')
                plot([1 length(event(c).rawData)], [20000 20000], 'y')
                hold off
            end
        end %if ~isempty(recording(c).annotations)
    end %c = 1:length(recording)    
end %for t = 1:length(capture_database)