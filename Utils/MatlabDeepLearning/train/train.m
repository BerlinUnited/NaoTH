clear all;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% CHANGE PARAMETERS HERE                                                  %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% select a meaningful name for the network
netName="CNN_aug1_synthetic";

% select (multiple) directories containing "ball"/"noball" subdirectories with 
% training data
digitDatasetPath = {...
    '../data/augmented/test1/', ...
    '../data/synthetic_ballonly'
    };

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% ACTUAL CODE: DO NOT CHANGE                                              %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

addpath('../generatorFunctions');

digitData = imageDatastore(digitDatasetPath, ...
        'IncludeSubfolders',true,...
        'LabelSource','foldernames', 'ReadFcn',@imgReader);


%%
% The labels for each patch is automatically determined by the name of the
% subfolder from digitDatasetPath the image is located at
countLabel = digitData.countEachLabel;
disp(countLabel)
numBallPatches = countLabel.Count(1)
%%
trainingNumFiles = round(numBallPatches*0.75);
%rng(1) % For reproducibility
[trainDigitData,testDigitData] = splitEachLabel(digitData, ...
				trainingNumFiles,'randomize');

layers = [imageInputLayer([16 16 1], 'DataAugmentation',{'randfliplr', 'randcrop'}) % ,'randcrop'
          %dropoutLayer()
          convolution2dLayer(5, 4, 'Stride', 2, 'Padding', 2)
          reluLayer
          maxPooling2dLayer(2, 'Stride', 2)
          %dropoutLayer()
          convolution2dLayer(3, 10, 'Stride', 1, 'Padding', 1)
          reluLayer
          maxPooling2dLayer(2, 'Stride', 1)
          %dropoutLayer()
          fullyConnectedLayer(64)
          reluLayer
          fullyConnectedLayer(2)
          softmaxLayer
          classificationLayer];

% Initialize the first convolutional layer weights using normally distributed 
% random numbers with standard deviation of 0.0001.
layer = layers(2);
N = (16*16 + 16*16*4)*0.5;
layer.Weights = 1/N * randn(layer.FilterSize(1), layer.FilterSize(2),1,layer.NumFilters);

layer = layers(5);
N = (8*8 + 8*8*10)*0.5;
layer.Weights = 1/N * randn(layer.FilterSize(1), layer.FilterSize(2),layers(2).NumFilters,layer.NumFilters);

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
    'LearnRateDropPeriod', 60, ...
    'L2Regularization', 0.004, ...
    'MaxEpochs', 30, ...
    'MiniBatchSize', 30); % 'cpu', 'parallel'

convnet = trainNetwork(digitData,layers,options);

%%
% create result output directory
resultFolder =  'result_' + netName;
mkdir(char(resultFolder));
save(resultFolder + '/convnet.mat', 'convnet');

% also create the final C++ class
createCppFile(convnet, char(resultFolder + '/' + netName));

% save the current train file as backup and "documentation"
copyfile('train.m', char(resultFolder + '/train.m'));
