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
%% Settings
% Choose the references here
%reference = reference_database.gen_from_go18_recordings;
reference = reference_database.gen_from_rc18_recordings;
event_names = fieldnames(capture_database);

threshold = 0.25;
window_size = 1024;
window_offset = 786;
%%
tp_bar = [];
fp_bar = [];
fn_bar = [];
for i=1:length(reference)
    fprintf("Check detection for reference %s \n", reference(i).name)
    
    % Check first event only for now
    event = capture_database.(event_names{6});
    tp = 0;
    fp = 0;
    fn = 0;
    
    annotated_capture = [];
    for capture_idx = 1:length(event)
        if ~isempty(event(capture_idx).annotations)
            annotated_capture(end+1) = capture_idx;
        end
    end
    
    for k=1:length(annotated_capture)
        % get raw capture data
        capture_data = event(annotated_capture(k)).rawData;
        result = slidingcrosscorrelation(capture_data, reference(i), window_size, window_offset,threshold);
        if result.detected
            for p = 1:length(result.positions)
                % it's classified as true positive if one sample in the sliding
                % window is marked as whistle label
                % its possible to detect a whistle multiple times in
                % correct or incorrect in a given recording
                if sum(event(annotated_capture(k)).annotations(result.positions:(result.positions + window_size - 1), 1) ) > 0
                    % timing correct -> true positive
                    tp = tp + 1;
                else
                    % timing incorrect -> false positive
                    fp = fp + 1;
                end
            end
        else
            % Nothing detected -> this is a false negative for this reference
            fn = fn + 1;
        % Note: true negatives are not tracked - how would one do that
        % anyway?
       end
    end
    
    fprintf("True Positives: %d \n", tp)
    fprintf("False Positives: %d \n", fp)
    fprintf("False Negatives: %d \n", fn)
    tp_bar(end+1) = tp;
    fp_bar(end+1) = fp;
    fn_bar(end+1) = fn;
    
    
end % end length(references)

%
% get the names
for q=1:length(reference)
    names(q,:) = string(reference(q).name);
end

% Plot the bar graphs
subplot(4,1,1);
bar(tp_bar)
ylim([0 length(annotated_capture)])
title('True Positives')

subplot(4,1,2);
bar(fp_bar)
ylim([0 length(annotated_capture)])
title('False Positives')

subplot(4,1,3);
bar(fn_bar)
ylim([0 length(annotated_capture)])
title('False Negatives')

subplot(4,1,4);
bar(length(annotated_capture) - fn_bar)
xtickangle(45)
xticks(1:length(names))
xticklabels(names)
ylim([0 length(annotated_capture)])
title('Number of whistles detected') % its more like number of detections

