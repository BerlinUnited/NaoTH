%% Copy this file to secret.m and adjust the paths
function secret()
    % Path where the capture_database.mat and other mat files are located
    databases_path = "<insert_path_here>/"; % should have slash at the end
    assignin('base','databases_path', databases_path)
    
    %% Path to the different databases
    
    % Insert capture db filename
    assignin('base','capture_database_path', char(strcat(databases_path, "<capture db filename>")))
    
    % Insert reference db filename
    assignin('base','reference_database_path', char(strcat(databases_path, "<reference db filename>")))    
    
    % Insert test recordings db filename
    assignin('base','tests_database_path', char(strcat(databases_path, "<test captures db filename>")))
    
    % Insert path to game recording databases
    assignin('base','game_database_path', char(strcat(databases_path, "<game captures db filename>")))
    %% Import folders for db creation scripts
    % Path to folder where the capture files are located.
    % The following folder structure under <subdir_to_import_captures> is expected:
    % capture_import_path
    % |-> event_name
    %   |-> game_name
    %       |-> half x
    %           |-> robot_name
    %               |-> raw files
    assignin('base','capture_import_path', char(strcat(databases_path, "<subdir_to_import_captures>/")))    
    
    % TODO beschreiben wie test recordings subfolder structure aussieht
    assignin('base','test_import_path', char(strcat(databases_path, "<subdir_to_import_test_recordings>/")))
    % TODO beschreiben wie import reference subfolder structure aussieht
    assignin('base','reference_import_path', char(strcat(databases_path, "<subdir_to_import_reference_whistles>")))   
    
    % TODO beschreiben wie game_import_path subfolder structure aussieht
    assignin('base','game_import_path', char(strcat(databases_path, "<subdir_to_import_reference_whistles>")))
    
    
    %% Paths for exporting the data from the databases
    assignin('base','capture_export_path', '<path_to_export_folder_for_captures>')
    assignin('base','figure_export_path', '<path_to_export_folder_for_figures>')
    
    assignin('base','reference_export_path', '<path_to_export_folder_for_references>')
    
    assignin('base','game_export_path', '<path_to_export_folder_for_game_recordings>')
end

