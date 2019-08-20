function convnet = transferTrain(convnet_org, retrainOptions, numLastLayersToRemove, lastLayersToAppend, trainData, dropoutProbabilities)
    % take all layers except the numLastLayersToRemove aount ones
    layersTransfer = convnet_org.Layers(1:end-numLastLayersToRemove);  
    % append new layers defined in lastLayersToAppend
    for i = 1:numel(lastLayersToAppend)
        layersTransfer(end+1) = lastLayersToAppend(i);
    end    
    %create empty array for nnet.cnn.layer.Layer objects
    layers = nnet.cnn.layer.ImageInputLayer.empty();
    % if any dropout probability is defined than is the use of dropoutlayers requested
    useDropout = numel(dropoutProbabilities) > 0;
    probabilityCount  = 1;
    for i = 1:numel(layersTransfer)
        layer = layersTransfer(i);
        probability = 0.5;
        switch class(layer)
            case 'nnet.cnn.layer.Convolution2DLayer'
                if useDropout
                    if probabilityCount <= numel(dropoutProbabilities)
                        probability = dropoutProbabilities(probabilityCount);
                        probabilityCount = probabilityCount + 1;
                    end
                    if probability > 0
                        fprintf('[added DropoutLayer(%1.4f)]\n', probability)
                        layers(end+1,1) = dropoutLayer(probability);
                    end
                end
                fprintf('[transfered Convolution2DLayer]\n');
                layers(end+1,1) = layer;
            case 'nnet.cnn.layer.FullyConnectedLayer'
                if useDropout
                    if probabilityCount <= numel(dropoutProbabilities)
                        probability = dropoutProbabilities(probabilityCount);
                        probabilityCount = probabilityCount + 1;
                    end
                    if probability > 0
                        fprintf('[added DropoutLayer(%1.4f)]\n', probability)
                        layers(end+1,1) = dropoutLayer(probability);
                    end
                end
                fprintf('[transfered FullyConnectedLayer]\n');
                layers(end+1,1) = layer;
            case 'nnet.cnn.layer.ImageInputLayer'
                fprintf('[transfered ImageInputLayer]\n');
                if numel(layer.DataAugmentation) > 0
                    layers(end+1,1) = imageInputLayer(layer.InputSize, 'DataAugmentation', layer.DataAugmentation);  
                else
                    layers(end+1,1) = imageInputLayer(layer.InputSize);
                end                                
            case 'nnet.cnn.layer.ReLULayer'
                fprintf('[transfered ReLULayer]\n');
                layers(end+1,1) = layer;
            case 'nnet.cnn.layer.MaxPooling2DLayer'
                fprintf('[transfered MaxPooling2DLayer]\n');
                layers(end+1,1) = layer;
            case 'nnet.cnn.layer.SoftmaxLayer'
                fprintf('[transfered SoftmaxLayer]\n');
                layers(end+1,1) = layer;
            case 'nnet.cnn.layer.ClassificationOutputLayer'
                fprintf('[transfered ClassificationOutputLayer]\n');
                layers(end+1,1) = layer;
            case 'nnet.cnn.layer.DropoutLayer'
                fprintf(2,'[skipping existing Dropout layer]\n');
%                 layers(end+1,1) = layer;
            otherwise
                fprintf(2,'[skipping unknown layer]\n');
        end
    end
    convnet = trainNetwork(trainData, layers, retrainOptions);    
end