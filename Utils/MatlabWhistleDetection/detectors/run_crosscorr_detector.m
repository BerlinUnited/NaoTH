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

%% config
windows_size = 1024;
window_offset = 786;

fftw('planner','estimate');

database = capture_database;

create_detection_plots = true;
create_non_detection_plots = false;

%% set references whistle set to use
% ref = reference_database.bhuman;
% ref = reference_database.kln17;
% ref = reference_database.rc17;
% ref = reference_database.go18;
%ref = reference_database.go17;
ref = reference_database.gen_from_go18_recordings;

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
    
    sprintf("Use captures from %s", cell2mat(recording_name))
    for c = 1:length(recording)
        sprintf("Check file %d of %d",c, length(recording))
        if ~isempty(recording(c).annotations)
            % get raw capture data
            capture_data = recording(c).rawData;
            record_name = strcat(recording(c).game_name, " ", recording(c).half, " ", recording(c).robot_name, " ", recording(c).filename);
            %% Max Cross Correlation Detector       
            threshold = 0.25;
            results = maxcrosscorr_detector(capture_data, windows_size, window_offset, threshold, ref, true);
            means = [];
            for i=1:window_offset:length(capture_data)
                if length(capture_data) - i >= windows_size
                    means(end + 1) = mean(abs(capture_data(i:i + windows_size - 1)));
                end
            end
            means = means / max(means);
            for r = 1:length(results)
                do_plots = (results(r).whistle_detected && create_detection_plots) || (~results(r).whistle_detected && create_non_detection_plots);

                if do_plots
                    % Plot results
                    fig = figure('name', "max cross correlation detector results", 'Visible', 'off');
                    hold on
                    plot(results(r).indices, results(r).values)
                    for i=1:length(results(r).indices)
                        plot([results(r).indices(i) results(r).indices(i) + windows_size - 1], [results(r).values(i) results(r).values(i)])
                    end
                    plot(recording(c).annotations(:,1), 'g')
                    plot(recording(c).annotations(:,2), 'r')
                    plot(results(r).positions, results(r).responses, 'mo')
                    plot(results(r).indices, means, 'm:')

                    plot([1 length(capture_data)], [0.25 0.25], 'y')
                    title(strrep(record_name, '_', '\_') )
                    hold off
                    subdir = "no_detections/";
                    if results(r).whistle_detected
                        subdir = "with_detections/";
                    end

                    if exist(strcat(figure_export_path, recording_name, "/", subdir, "/", results(r).name, "/threshold_", num2str(threshold)), 'dir') ~= 7
                        mkdir(char(strcat(figure_export_path, recording_name, "/", subdir, "/", results(r).name, "/threshold_", num2str(threshold))))
                    end
                    filepath = strcat(figure_export_path, recording_name, "/", subdir, results(r).name, "/threshold_", num2str(threshold), "/", record_name);
                    filepath = strrep(filepath,' ', '_');
                    filepath = strrep(filepath, 'raw', 'png');
                    try
                        saveas(fig, char(filepath), 'png');
                    catch
                        disp(strcat("could not save figure for ", filepath))
                    end
                end
                
                if results(r).whistle_detected
                    for p = 1:length(results(r).positions)
                        if sum( recording(c).annotations(results(r).positions(p):(results(r).positions(p) + windows_size - 1), 1) ) > 0
                            ref_stat.(char(ref(r).name)).tp = ref_stat.(char(ref(r).name)).tp + 1;
                        else
                            ref_stat.(char(ref(r).name)).fp = ref_stat.(char(ref(r).name)).fp + 1;
                        end
                    end
                end                   
            end
        end %if ~isempty(recording(c).annotations)
    end %c = 1:length(recording)
    %%
    % Generate the TP/FP plots
    tps = [];
    fps = [];
    for r = 1:length(ref)
        % TODO should generate a bar even if there were no detections
        if ref_stat.(char(ref(r).name)).tp >= 0 || ref_stat.(char(ref(r).name)).fp >= 0
            tps(end + 1) = ref_stat.(char(ref(r).name)).tp;
            fps(end + 1) = ref_stat.(char(ref(r).name)).fp;
        end
    end
    tick_labels = strings(length(tps));
    tick = 1;
    for r = 1:length(ref)
        if ref_stat.(char(ref(r).name)).tp >= 0 || ref_stat.(char(ref(r).name)).fp >= 0
            tick_labels(tick) = strrep(ref(r).name, '_', '\_');
            tick = tick + 1;
        end
    end
    max_count = max([max(tps) max(fps)]);
    figure('name', strcat("comparison TP and FP ", recording_name, " threshold: ", num2str(threshold) ))
    if ~isempty(tps)
        subplot(1,2,1)
        bar(tps)
        title('TP')
        xtickangle(45)
        xticks(1:length(tick_labels))
        xticklabels(tick_labels)
        ylabel('total event count')
        %ylim([0 max_count + 1])
        ylim([0 100])
    end
    if ~isempty(fps)
        subplot(1,2,2)
        bar(fps)
        title('FP')
        xtickangle(45)
        xticks(1:length(tick_labels))
        xticklabels(tick_labels)
        ylabel('total event count')
        ylim([0 max_count + 1])  
    end
    return
end %for t = 1:length(capture_database)