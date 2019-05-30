addpath('../tools');
addpath('../generatorFunctions');

clear all;

% select a meaningful name for the network
netName = "CNN_ND_aug2_full_conv_test45";

% load data
[data, labels] = load_from_mat('../data/basic.mat', '../data/augmented_rc17.mat');

target = zeros(size(data,4),2);
target(:,1) = (labels == 'ball');
target(:,2) = (labels == 'noball');



% select (multiple) directories containing "ball"/"noball" subdirectories with 
% training data
%{
dataSetPath = {...
    '../data/augmented/test2/' ...
    };

addpath('../tools');
ball = read_from_one_image('../data/rc17_1_ball.png');
noball = read_from_one_image('../data/rc17_1_noball.png');
data = cat(4,ball,noball);
size(data)
labels = zeros(size(data,4),2);
labels(1:size(ball,4),1) = 1;
labels((size(ball,4)+1):end,2) = 1;
%labels = [ones(size(ball,4),1);zeros(size(noball,4),1)];

%labels = categorical(labels,[1,0],{'ball','noball'});
%}

% check if output directory already exists
resultFolder =  'result_' + netName;
if isdir(resultFolder)
    error('Aborting because output directory "%s" already exists!', resultFolder)
end


%numBallPatches = countLabel.Count(1)
%%
%trainingNumFiles = round(numBallPatches*0.75);
%rng(1) % For reproducibility
%[trainData,testData] = splitEachLabel(data, ...
%				trainingNumFiles,'randomize');

layers = [imageInputLayer([16 16 1], 'DataAugmentation',{'randfliplr', 'randcrop'})
          dropoutLayer(0.1)
          convolution2dLayer(5, 4, 'Stride', 2, 'Padding', 2)
          maxPooling2dLayer(4, 'Stride', 2, 'Padding', 1)
          dropoutLayer(0.25)
          convolution2dLayer(3, 4, 'Stride', 1, 'Padding', 1)
          reluLayer
          dropoutLayer()
          convolution2dLayer(3, 12, 'Stride', 2, 'Padding', 1)
          reluLayer
          convolution2dLayer(2, 2, 'Stride', 1, 'Padding', 0)
          regressionLayer];

% Initialize the first convolutional layer weights using normally distributed 
% random numbers with standard deviation of 0.0001.
layer = layers(3);
N = (16*16 + 16*16*4)*0.5;
layer.Weights = 1/N * randn(layer.FilterSize(1), layer.FilterSize(2),1,layer.NumFilters);

layer = layers(6);
N = (8*8 + 8*8*12)*0.5;
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
    'LearnRateDropFactor', 0.5, ...
    'LearnRateDropPeriod', 1, ...
    'MaxEpochs', 3, ...
    'MiniBatchSize', 32); % 'cpu', 'parallel'     %'L2Regularization', 0.004, ...

convnet = trainNetwork(data, target, layers, options);

% create the output directory if necessary
[~, errorMsg, ~] = mkdir(char(resultFolder));
if ~isempty(errorMsg)
    error('Aborting because output directory "%s" already exists!', resultFolder)
end

%%
% save the resulting model
save(resultFolder + '/convnet.mat', 'convnet');

% also create the final C++ class
%createCppFile(convnet, char(resultFolder + '/' + netName));

% save the current train file as backup and "documentation"
copyfile('trainNaoDevils.m', char(resultFolder + '/train.m'));
