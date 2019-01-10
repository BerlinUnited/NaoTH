function preLabeling( srcDir )
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % This script creates a json file like the python label script does. 
    % The json file along with the original log file can be used in
    % the python script to fix the label mistakes
    
    % srcDir can look like the following path, where data can hold subfolders
    % for the the classes 'ball' and 'noball'
    %srcDir = 'C:\Projekte\_patch_data\2017-07-25_RC17\2017-07-30_12-15-00_Berlin_United_vs_Camellia_Dragons_half2\game_logs\1_91_Nao0379\data';
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    addpath('../train');
    % Set the convolutional net used for classifying
    load('../generated/result_CNN_ND_all_91_retrained_small_5_epochs/convnet.mat', 'convnet')
    net = convnet;
    

    % Load images
    digitData = imageDatastore(srcDir, ...
        'IncludeSubfolders',true,'LabelSource','foldernames', 'ReadFcn',@imgReader);

    % classify the images
    classified_data = classify(net,digitData);

    % %
    noball_names = [];
    ball_names = [];
    files = digitData.Files; % for speed up
    % 
    noball_counter = 1;
    ball_counter = 1;
    for i = 1:length(classified_data)
        [~,name,~] = fileparts(files{i});
        test = str2double(name);
        if classified_data(i) == 'noball'
           noball_names(noball_counter) = test;
           noball_counter = noball_counter + 1;
        elseif classified_data(i) == 'ball'
            ball_names(ball_counter) = test;
            ball_counter = ball_counter + 1;
        end
    end

    % Output the json file
    outputfile = fullfile(srcDir, 'ball_patch.json');
    json_labels = jsonencode(struct('noball',noball_names, 'ball',ball_names));
    fileID = fopen(outputfile,'w');
        fprintf(fileID,'%s', json_labels);
    fclose(fileID);
end