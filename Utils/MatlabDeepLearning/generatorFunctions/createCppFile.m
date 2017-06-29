function [path] = createCppFile(convnet, file)

fileID = fopen(file, 'w');

layers = convnet.Layers;

for i = 1:numel(layers)
   switch class(layers(i))
    case 'nnet.cnn.layer.ImageInputLayer'
       addImageInputLayer(fileID,i,layers(i));
       
       rows = layers(i).InputSize(1);
       cols = layers(i).InputSize(2);
       
       %display 1;
    case 'nnet.cnn.layer.Convolution2DLayer'
       [rows, cols] = addConvolution2Dlayer(fileID,i,layers(i),rows,cols);
       %display 2;
    case 'nnet.cnn.layer.ReLULayer'
       addReLULayer(fileID,i,rows,cols);
       %display 3;
    case 'nnet.cnn.layer.MaxPooling2DLayer'
       %dummy();
       display 4;
    case 'nnet.cnn.layer.FullyCocnnectedLayer'
       [rows,cols] = addFullyConnectedLayer(fileID,i,layers(i),rows,cols);
       display 5;
    case 'nnet.cnn.layer.SoftmaxLayer'
       %dummy();
       display 6;
    case 'nnet.cnn.layer.ClassificationOutputLayer'
       %dummy();
       display 7;
    otherwise
   end
end

end
