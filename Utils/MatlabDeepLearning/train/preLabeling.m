clear all;
tic;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This script creates a json file like the python label script does. The
% folders and the json file along with the original log file can be used in
% the python script to fix the label mistakes
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Set the convolutional net used for classifying
load cn4.mat
net = convnet;
%Set the root folder of the images to be labeled
srcDir = 'data/unlabeled';

% Load iamges
digitData = imageDatastore(srcDir, ...
    'IncludeSubfolders',true,'LabelSource','foldernames', 'ReadFcn',@imgReader);

% classify the images
classified_data = classify(net,digitData);

%
noball_counter = 1;
ball_counter = 1;
for i = 1:length(classified_data)
    [~,name,~] = fileparts(digitData.Files{i});
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
json_labels = jsonencode(struct('noball',noball_names, 'ball',ball_names));
fileID = fopen('data/game.json','w');
fprintf(fileID,'%s', json_labels);
fclose(fileID);
toc;