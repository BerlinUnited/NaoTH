clear all;

digitDatasetPath = {...
    '../data/16x16_bw/rasen-ball-robot-move-5-best-24x24',...
    '../data/16x16_bw/2016-06-29-patches-play' };

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
rng(1) % For reproducibility
[trainDigitData,testDigitData] = splitEachLabel(digitData, ...
				trainingNumFiles,'randomize');
            
            
layers = [imageInputLayer([16 16 1], 'DataAugmentation',{'randfliplr','randcrop'})
          convolution2dLayer(3, 8, 'Stride', 2)
          reluLayer
          convolution2dLayer(3, 10, 'Stride', 2)
          reluLayer
          fullyConnectedLayer(2)
          softmaxLayer
          classificationLayer()];
      
options = trainingOptions('sgdm',...
    'MaxEpochs',150,...
	'InitialLearnRate',0.001,...
    'ExecutionEnvironment', 'parallel'); % 'cpu', 'parallel'

convnet = trainNetwork(trainDigitData,layers,options);

save cn4.mat convnet

