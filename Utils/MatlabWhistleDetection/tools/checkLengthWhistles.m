clear variables
clc
%% init defaults
addpath('../common')
secret()

try
    load(capture_database_path)
catch
    disp('ERROR: Could not load capture database')
    return
end
%%
% use annotations to check how long the captured whistles are
whistle_lengths = [];

database = capture_database;
event_names = fieldnames(database);
for t = 1:length(event_names)
    event_name = event_names(t);
    event_captures = database.(event_name{1});
    
    for c = 1:length(event_captures)
        if ~isempty(event_captures(c).annotations)
            % assumes that max one whistle is marked per capture
            whistle_marker = event_captures(c).annotations(:,1);
            whistle_length = sum(whistle_marker);
            whistle_lengths = [whistle_lengths whistle_length];
            
            % 
            if whistle_length > 9000
                record_name = strcat(event_captures(c).game_name, " ", event_captures(c).half, " ", event_captures(c).robot_name, " ", event_captures(c).filename);
                disp(record_name)
                %plot(event_captures(c).rawData(logical(whistle_marker)))
                %sound(event_captures(c).rawData(logical(whistle_marker)), 8000)
                return
            end
        end
    end
    return
end

%%
% plot the histogram of whistle lengths
% plot(whistle_lengths)