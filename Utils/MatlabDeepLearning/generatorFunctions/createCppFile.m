function [path] = createCppFile(convnet, file)

fileID = fopen(file, 'w');

layers = convnet.Layers;

for i = 1:numel(layers)
   fprintf('%d make %s',i, class(layers(i)));
   
   switch class(layers(i))
    case 'nnet.cnn.layer.ImageInputLayer'
       addImageInputLayer(fileID,i,layers(i));
       
       rows = layers(i).InputSize(1);
       cols = layers(i).InputSize(2);
       
       fprintf(' [done]');
    case 'nnet.cnn.layer.Convolution2DLayer'
       [rows, cols] = addConvolution2Dlayer(fileID,i,layers(i),rows,cols);
       fprintf(' [done]');
    case 'nnet.cnn.layer.ReLULayer'
       addReLULayer(fileID,i,rows,cols);
       fprintf(' [done]');
    case 'nnet.cnn.layer.MaxPooling2DLayer'
       %dummy();
       fprintf(' [missing]');
    case 'nnet.cnn.layer.FullyConnectedLayer'
       [rows,cols] = addFullyConnectedLayer(fileID,i,layers(i),rows,cols);
       fprintf(' [done]');
    case 'nnet.cnn.layer.SoftmaxLayer'
       addSoftMaxLayer(fileID,i,rows,cols)
       fprintf(' [done]');
    case 'nnet.cnn.layer.ClassificationOutputLayer'
       %dummy();
       fprintf(' [missing]');
    otherwise
   end
   
   fprintf('\n');
end

end
