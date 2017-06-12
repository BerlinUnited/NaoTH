clear all;
%digitDatasetPath = fullfile(matlabroot,'toolbox','nnet','nndemos', ...
%        'nndatasets','DigitDataset');
digitDatasetPath = 'D:\RoboCup\OtherRepos\MatlabCNN\image_set01';
digitData = imageDatastore(digitDatasetPath, ...
        'IncludeSubfolders',true,'LabelSource','foldernames');
    
figure;
perm = randperm(10000,20);
for i = 1:20
    subplot(4,5,i);
    imshow(digitData.Files{perm(i)});
end

CountLabel = digitData.countEachLabel;

img = readimage(digitData,1);
size(img)

trainingNumFiles = 750;
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
 
YTest = classify(convnet,digitData_noball)
