clear variables;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This script creates a json file like the python label script does. The
% folders and the json file along with the original log file can be used in
% the python script to fix the label mistakes
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
addpath('../train');
% Set the convolutional net used for classifying
load('../generated/result_CNN_rc17_augmented_4/convnet.mat')
net = convnet;
%Set the root folder of the images to be labeled
srcDir = 'D:\Downloads\RC17Games\2017-07-30_UPennalizers\half2\090917-0208-Nao0377-labeled\game\';

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
outputfile = fullfile(srcDir, 'game.json');
json_labels = jsonencode(struct('noball',noball_names, 'ball',ball_names));
fileID = fopen(outputfile,'w');
    fprintf(fileID,'%s', json_labels);
fclose(fileID);