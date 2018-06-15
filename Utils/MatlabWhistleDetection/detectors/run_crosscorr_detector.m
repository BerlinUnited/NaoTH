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
threshold = 0.25;
%threshold = 0.4;
% flip = false;
flip = true;

fftw('planner','estimate');

database = capture_database;

create_detection_plots = true;
create_non_detection_plots = false;

%% set references whistle set to use
% ref_set_name = 'bhuman';
% ref_set_name = 'kln17';
% ref_set_name = 'rc17';
% ref_set_name = 'go18';
ref_set_name = 'go17';
%ref_set_name = 'gen_from_go18_recordings';

ref = reference_database.(ref_set_name);

ref_stat_tpl = struct;
for r = 1:length(ref)
    % replace points and minus signs with underscore
    ref(r).name = strrep(ref(r).name, '-', '_');
    ref(r).name = strrep(ref(r).name, '.', '_');
    
    ev  = struct;
    ev.tp = 0;
    ev.fp = 0;
    ev.valid = false;
    if ref(r).autocorrelation ~= 0
        ev.valid = true; 
    end

    ref_stat_tpl.(char(ref(r).name)) = ev;
end  
%%
recordings_names = fieldnames(database);
for t = 1:length(recordings_names)
    ref_stat = ref_stat_tpl;
    has_any_annotation = false;
    recording_name = recordings_names(t);
    recording = database.(recording_name{1});
    disp(sprintf("Use captures from %s", cell2mat(recording_name)))
    for c = 1:length(recording)
        disp(sprintf("Check file %d of %d", c, length(recording)))
        if ~isempty(recording(c).annotations)
            has_any_annotation = true;
            % get raw capture data
            capture_data = recording(c).rawData;
            record_name = strcat(recording(c).game_name, " ", recording(c).half, " ", recording(c).robot_name, " ", recording(c).filename);
            %% Max Cross Correlation Detector       
            results = maxcrosscorr_detector(capture_data, windows_size, window_offset, threshold, ref, flip);
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

                    plot([1 length(capture_data)], [threshold threshold], 'y')
                    title(strrep(record_name, '_', '\_') )
                    hold off
                    subdir = "no_detections/";
                    if results(r).whistle_detected
                        subdir = "with_detections/";
                    end
                    if flip
                        subdir = strcat("flipped_", subdir);
                    end                    
                    
                    if exist(strcat(figure_export_path, recording_name, "/ref_", ref_set_name, "/", subdir, "/", results(r).name, "/th_", num2str(threshold)), 'dir') ~= 7
                        mkdir(char(strcat(figure_export_path, recording_name, "/ref_", ref_set_name, "/", subdir, "/", results(r).name, "/th_", num2str(threshold))))
                    end
                    filepath = strcat(figure_export_path, recording_name, "/ref_", ref_set_name, "/", subdir, results(r).name, "/th_", num2str(threshold), "/", record_name);
                    filepath = strrep(filepath,' ', '_');
                    filepath = strrep(filepath, '.raw', '_raw');
                    try
                        saveas(fig, char(filepath), 'png');
                    catch
                        disp(strcat("could not save figure for ", filepath))
                    end
                    clf(fig);
                    close(fig);
                    clear fig filepath
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
            clear means results
        end %if ~isempty(recording(c).annotations)
    end %c = 1:length(recording)
    if has_any_annotation
        %%
        % Generate the TP/FP plots
        tps = [];
        fps = [];
        tps1 = [];
        fps1 = [];
        tps2 = [];
        fps2 = [];
        tick_labels = strings(0);
        tick_labels1 = strings(0);
        tick_labels2 = strings(0);
        for r = 1:length(ref)
            if ~ref_stat.(char(ref(r).name)).valid
                continue
            end
            % TODO should generate a bar even if there were no detections
            if ref_stat.(char(ref(r).name)).tp > 0 || ref_stat.(char(ref(r).name)).fp > 0
                tps(end + 1) = ref_stat.(char(ref(r).name)).tp;
                fps(end + 1) = ref_stat.(char(ref(r).name)).fp;
                tick_labels(end + 1) = strrep(ref(r).name, '_', '\_');
            end
            if ref_stat.(char(ref(r).name)).tp > 0 && ref_stat.(char(ref(r).name)).fp == 0
                tps1(end + 1) = ref_stat.(char(ref(r).name)).tp;
                fps1(end + 1) = ref_stat.(char(ref(r).name)).fp;
                tick_labels1(end + 1) = strrep(ref(r).name, '_', '\_');
            end
            if ref_stat.(char(ref(r).name)).fp > 0
                tps2(end + 1) = ref_stat.(char(ref(r).name)).tp;
                fps2(end + 1) = ref_stat.(char(ref(r).name)).fp;
                tick_labels2(end + 1) = strrep(ref(r).name, '_', '\_');
            end
        end
        fig = figure('name', strcat("comparison TP and FP ", recording_name, " threshold: ", num2str(threshold)), 'Position', get(0,'ScreenSize'), 'Visible', 'off');
        max_count = max([max(tps) max(fps)]);
        if ~isempty(tps)
            subplot(2,2,1)
            bar(tps)
            title('TP all')
            xtickangle(45)
            xticks(1:length(tick_labels))
            xticklabels(tick_labels)
            ylabel('total event count')
            ylim([0 max_count + 1])
    %         ylim([0 100])
        end
        if ~isempty(fps)
            subplot(2,2,2)
            bar(fps)
            title('FP all')
            xtickangle(45)
            xticks(1:length(tick_labels))
            xticklabels(tick_labels)
            ylabel('total event count')
            ylim([0 max_count + 1])  
        end  
        max_count = max([max(tps1) max(fps1)]);
        if ~isempty(tps1)
            subplot(2,2,3)
            bar(tps1)
            title('has TP only')
            xtickangle(45)
            xticks(1:length(tick_labels1))
            xticklabels(tick_labels1)
            ylabel('total event count')
            ylim([0 max_count + 1])
    %         ylim([0 100])
        end
        max_count = max([max(tps2) max(fps2)]);
        if ~isempty(fps2)
            subplot(2,2,4)
            bar(fps2)
            title('has FP')
            xtickangle(45)
            xticks(1:length(tick_labels2))
            xticklabels(tick_labels2)
            ylabel('total event count')
            ylim([0 max_count + 1])  
        end
        figtitle = "comparison_TP_FP_";
        if flip
            figtitle = strcat(figtitle, "flipped_");
        end                    
        filepath = strcat(figure_export_path, recording_name, "/ref_", ref_set_name, "/", figtitle, num2str(threshold), ".png");
        try
            saveas(fig, char(filepath), 'png');
        catch
            disp(strcat("could not save figure for ", filepath))
        end
        clf(fig);
        close(fig);
        clear fig filepath tps fps tps1 fps1 tps2 fps2 tick_labels tick_labels1 tick_labels2
    end
    clear ref_stat
end %for t = 1:length(capture_database)
 