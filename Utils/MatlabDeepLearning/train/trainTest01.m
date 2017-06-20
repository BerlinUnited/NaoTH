clear all;

digitDatasetPath = '..\data\16x16_bw\2016-06-28-patches-play-no-ball';
digitData = imageDatastore(digitDatasetPath, ...
        'IncludeSubfolders',true,'LabelSource','foldernames');

% show sample patch data
figure;
perm = randperm(length(digitData.Files),20);
for i = 1:20
    subplot(4,5,i);
    imshow(digitData.Files{perm(i)});
end
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
            
            
layers = [imageInputLayer([16 16 1])
          convolution2dLayer(3,1)
          reluLayer
          maxPooling2dLayer(2,'Stride',2)
          fullyConnectedLayer(2)
          softmaxLayer
          classificationLayer()];
      
options = trainingOptions('sgdm','MaxEpochs',150, ...
	'InitialLearnRate',0.0001);

convnet = trainNetwork(trainDigitData,layers,options);

%YTest = classify(convnet,testDigitData);
%TTest = testDigitData.Labels;

%accuracy = sum(YTest == TTest)/numel(TTest)

digitData_noball = imageDatastore('D:\RoboCup\OtherRepos\MatlabCNN\image_set01\noball', ...
        'IncludeSubfolders',true,'LabelSource','foldernames');
 
YTest = classify(convnet,digitData_noball);
