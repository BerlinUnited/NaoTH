function convnets = transferTrainMultiple(convnet, netName, transferOptions, numLayerstoRemove, lastLayers, trainDatasetPaths, dropOutLayersToAdd, testDatasetPath)

sP = size(trainDatasetPaths);
sO = size(transferOptions);
if(sP(1) ~= sO(1))
    disp('amount of paths and options do not match!')
    return
end

sL = size(lastLayers);
if(sP(1) ~= sL(1))
    disp('amount of paths and new layer definitions do not match!')
    return
end

quality(convnet, strcat(netName, ' before transfer training'), testDatasetPath);

convnets = {};
for idx = 1:sP(1)
    data = imageDatastore(trainDatasetPaths{idx}, ...
            'IncludeSubfolders',true,'LabelSource','foldernames');

    % determine amount of classes in current dataset
    countLabel = data.countEachLabel;
    disp(countLabel)

    % sample on howto execute tranfer training without any dropoutlayers
    %convnet_nodropout = transferTrain(convnet, optionsTransfer, 3, lastLayers, digitData, []);

    % sample on howto execute tranfer training without dropoutlayers before convolution or fully
    % connected layers
    convnetTransfer = transferTrain(convnet, transferOptions{idx}, numLayerstoRemove(idx), lastLayers{idx}, data, dropOutLayersToAdd);
    convnets{end+1} = convnetTransfer;
    
    quality(convnetTransfer, strcat(netName, ' after transfer No.', idx), testDatasetPath);
end






end