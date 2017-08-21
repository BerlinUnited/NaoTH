clear all;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% CHANGE PARAMETERS HERE                                                  %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% select a meaningful name for the network
netName="CNN_synth_full_conv";

% select (multiple) directories containing "ball"/"noball" subdirectories with 
% training data
dataSetPath = {...
    '../data/synthetic/' ...
    };

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% ACTUAL CODE: DO NOT CHANGE                                              %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
addpath('../generatorFunctions');

% create result output directory and fail if output directory already exists
resultFolder =  'result_' + netName;
[~, errorMsg, ~] = mkdir(char(resultFolder));
if ~isempty(errorMsg)
    error('Aborting because output directory "%s" already exists!', resultFolder)
end


data = imageDatastore(dataSetPath, ...
        'IncludeSubfolders',true,...
        'LabelSource','foldernames', 'ReadFcn',@imgReader);


%%
% The labels for each patch is automatically determined by the name of the
% subfolder from dataSetPath the image is located at
countLabel = data.countEachLabel;
disp(countLabel)
numBallPatches = countLabel.Count(1)
%%
%trainingNumFiles = round(numBallPatches*0.75);
%rng(1) % For reproducibility
%[trainData,testData] = splitEachLabel(data, ...
%				trainingNumFiles,'randomize');

layers = [imageInputLayer([16 16 1], 'DataAugmentation',{'randfliplr', 'randcrop'}) % ,'randcrop'
          dropoutLayer(0.05)
          convolution2dLayer(5, 4, 'Stride', 2, 'Padding', 2)
          reluLayer
          maxPooling2dLayer(2, 'Stride', 2)
          dropoutLayer(0.025)
          convolution2dLayer(3, 10, 'Stride', 1, 'Padding', 1)
          reluLayer
          maxPooling2dLayer(2, 'Stride', 1)
          dropoutLayer(0.01)
          %fullyConnectedLayer(64)
          convolution2dLayer(3, 2, 'Stride', 1, 'Padding', 0)
          reluLayer
          %fullyConnectedLayer(2)
          softmaxLayer
          classificationLayer];

% Initialize the first convolutional layer weights using normally distributed 
% random numbers with standard deviation of 0.0001.
layer = layers(3);
N = (16*16 + 16*16*4)*0.5;
layer.Weights = 1/N * randn(layer.FilterSize(1), layer.FilterSize(2),1,layer.NumFilters);

layer = layers(7);
N = (8*8 + 8*8*10)*0.5;
layer.Weights = 1/N * randn(layer.FilterSize(1), layer.FilterSize(2),layers(3).NumFilters,layer.NumFilters);

%https://www.mathworks.com/help/vision/examples/object-detection-using-deep-learning.html
%{
opts = trainingOptions('sgdm', ...
    'Momentum', 0.9, ...
    'InitialLearnRate', 0.001, ...
    'LearnRateSchedule', 'piecewise', ...
    'LearnRateDropFactor', 0.1, ...
    'LearnRateDropPeriod', 8, ...
    'L2Regularization', 0.004, ...
    'MaxEpochs', 40, ...
    'MiniBatchSize', 128, ...
    'Verbose', true);
%}

options = trainingOptions('sgdm',...
    'Momentum', 0.9, ...
    'InitialLearnRate', 0.001, ...
    'LearnRateSchedule', 'piecewise', ...
    'LearnRateDropFactor', 0.1, ...
    'LearnRateDropPeriod', 10, ...
    'L2Regularization', 0.004, ...
    'MaxEpochs', 30, ...
    'MiniBatchSize', 30); % 'cpu', 'parallel'

convnet = trainNetwork(data,layers,options);

%%
% save the resulting model
save(resultFolder + '/convnet.mat', 'convnet');

% also create the final C++ class
createCppFile(convnet, char(resultFolder + '/' + netName));

% save the current train file as backup and "documentation"
copyfile('train.m', char(resultFolder + '/train.m'));
