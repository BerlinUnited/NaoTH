clear all;

digitDatasetPath = {...
    '../data/16x16_bw/rasen-ball-robot-move-5-best-24x24',...
    '../data/16x16_bw/2016-06-29-patches-play' };

digitDatasetPath = {...
    '../data/16x16_bw/small-set'};

digitData = imageDatastore(digitDatasetPath, ...
        'IncludeSubfolders',true,...
        'LabelSource','foldernames');


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
          convolution2dLayer(3, 8,'Stride',2)
          reluLayer
          convolution2dLayer(3, 10,'Stride',2)
          reluLayer
          fullyConnectedLayer(64)
          reluLayer
          fullyConnectedLayer(2)
          softmaxLayer
          classificationLayer];

% Initialize the first convolutional layer weights using normally distributed 
% random numbers with standard deviation of 0.0001.
layers(2).Weights = 0.0001 * randn(size(layers(2).Weights));

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
    'MaxEpochs',100,...
    'MiniBatchSize',30,...
    'ExecutionEnvironment', 'cpu'); % 'cpu', 'parallel'

convnet = trainNetwork(digitData,layers,options);

save cn11.mat convnet

