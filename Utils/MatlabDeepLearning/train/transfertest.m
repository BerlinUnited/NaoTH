clear all;

% load a mat file containing a trained CNN named convnet
load('D:\Projekte\Robots\MatlabDeepLearning\test11cnn.mat')
 
% set path and load dataset for further training
datasetPath = '../data/16x16_bw/2016-06-29-patches-play';
data = imageDatastore(datasetPath, ...
        'IncludeSubfolders',true,'LabelSource','foldernames');

% set options for training 
optionsTransfer = trainingOptions('sgdm' ...
    ,'L2Regularization', 0.004 ...
    ,'MaxEpochs',1 ...
    ,'InitialLearnRate',0.0001...
    ,'ExecutionEnvironment', 'parallel'...
);

% determine amount of classes in current dataset
countLabel = data.countEachLabel;
disp(countLabel)
[labelAmount t] = size(countLabel);

% define new last layers
lastLayers = [
    fullyConnectedLayer(labelAmount)
    softmaxLayer
    classificationLayer()
];

% sample on howto execute tranfer training without any dropoutlayers
%convnet_nodropout = transferTrain(convnet, optionsTransfer, 3, lastLayers, digitData, []);

% sample on howto execute tranfer training without dropoutlayers before convolution or fully
% connected layers
convnetTransfer = transferTrain(convnet, optionsTransfer, 3, lastLayers, data, [0.1 0.15 0 0.5]);








