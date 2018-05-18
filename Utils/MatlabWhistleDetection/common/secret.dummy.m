function secret()
    databases_path = "<path_to_databases>/";
    assignin('base','databases_path', databases_path)

    assignin('base','tests_database_path', char(strcat(databases_path, "<test_captures_database>.mat")))
    assignin('base','capture_database_path', char(strcat(databases_path, "<capture_database>.mat")))
    assignin('base','reference_database_path', char(strcat(databases_path, "<reference_database>.mat")))

    assignin('base','test_import_path', char(strcat(databases_path, "<subdir_to_import_test_recordings>/")))
    assignin('base','capture_import_path', char(strcat(databases_path, "<subdir_to_import_captures>/")))
    assignin('base','reference_import_path', char(strcat(databases_path, "<subdir_to_import_reference_whistles>")))

    assignin('base','capture_export_path', '<path_to_export_folder_for_captures>')
    assignin('base','figure_export_path', '<path_to_export_folder_for_figures>')

    %TODO create more variables for other things than capture databases
end

