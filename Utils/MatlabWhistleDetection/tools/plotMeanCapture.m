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
%% 
testBla = capture_database.go17_recordings(1).rawData;
fig = figure();
plot(abs(testBla))
hold on
plot([1 length(testBla)], [mean(abs(testBla)) mean(abs(testBla))], 'y')
%%
sound(testBla, 8000)