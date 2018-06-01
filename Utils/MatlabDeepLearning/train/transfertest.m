clear all;

% load a mat file containing a trained CNN named convnet
load('../generated\result_CNN_aug1_synthetic/convnet.mat')
 
testDatasetPath = 'M:/Experiments/Matlab Deep Learning/data_RC17/tk-test-complete';

% set path and load dataset for further training
trainDatasetPaths = {
                'M:/Experiments/Matlab Deep Learning/data_RC17/basic/2016-06-28-patches-play'
                'M:/Experiments/Matlab Deep Learning/data_RC17/basic/2016-06-29-patches-play'
                };

% set options for training 
optionsTransfer = {
    trainingOptions('sgdm' ...
    ,'L2Regularization', 0.004 ...
    ,'MaxEpochs',1 ...
    ,'InitialLearnRate',0.0001...
    ,'ExecutionEnvironment', 'parallel'...
    )
    
    trainingOptions('sgdm' ...
    ,'L2Regularization', 0.004 ...
    ,'MaxEpochs',1 ...
    ,'InitialLearnRate',0.0001...
    ,'ExecutionEnvironment', 'parallel'...
    )
};

% define new last layers
lastLayers = {
     [
%     fullyConnectedLayer(2)
%     softmaxLayer
%     classificationLayer()
     ]
     [
%     fullyConnectedLayer(2)
%     softmaxLayer
%     classificationLayer()
     ]
};

numLayerstoRemove = [0 0];

% sample on howto use tranfer training with dropoutlayers before
% convolution or fully connected layers
%dropOutLayersToAdd = [[0.1 0.15 0 0.5] ]

% sample on howto use tranfer training without any dropoutlayers
dropOutLayersToAdd = [[] []];


%sample on transfer training only multiple times
convnetTransfer = transferTrainMultiple(convnet ,'test11cnn', optionsTransfer, numLayerstoRemove, lastLayers, trainDatasetPaths, dropOutLayersToAdd, testDatasetPath);

%sample on transfer training only one time
%convnet_nodropout = transferTrain(convnet, optionsTransfer(1), 3, lastLayers(1), digitData, []);










