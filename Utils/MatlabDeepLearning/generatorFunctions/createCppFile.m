function [path] = createCppFile(convnet, file, isKerasModel)

if exist('isKerasModel', 'var') && isKerasModel
    subtractAvg = false;
else
    subtractAvg = true;
end

BodyFile   = fopen(strcat(file,'.cpp'), 'w');
HeaderFile = fopen(strcat(file,'.h'), 'w');

[~,name,~] = fileparts(file);

fprintf(HeaderFile,strcat('#ifndef _',name,'_H_\n'));
fprintf(HeaderFile,strcat('#define _',name,'_H_\n\n'));

fprintf(HeaderFile,'#include <limits>\n\n');
fprintf(HeaderFile,'#include <fstream>\n\n');

fprintf(HeaderFile,'#include "AbstractCNNClassifier.h"\n\n');

fprintf(HeaderFile,'class %s : public AbstractCNNClassifier\n{\n\n',name);

fprintf(HeaderFile,'public:\n');

% DEBUG:
% add output stream for testing
%{
fprintf(HeaderFile,'\t%s(){out.open("example.txt");}\n\n',name);
fprintf(HeaderFile,'\t~%s(){out.close();}\n\n',name);
fprintf(HeaderFile,'std::ofstream out;\n');
%}

%fprintf(HeaderFile,'\tbool classify( float* /*BallCandidates::Patch&*/ p/*, naoth::CameraInfo::CameraID cameraId*/);\n\n');
fprintf(HeaderFile,'\tbool classify(const BallCandidates::Patch& p);\n\n');

fprintf(HeaderFile,'private:\n');

fprintf(BodyFile,strcat('#include "',name,'.h"\n\n'));
%fprintf(BodyFile,'bool %s::classify( float* /*BallCandidates::Patch&*/ p/*, naoth::CameraInfo::CameraID cameraId*/){\n',name);
fprintf(BodyFile,'bool %s::classify(const BallCandidates::Patch& p){\n',name);

layers = convnet.Layers;
level = 1;
for i = 1:numel(layers)
   
   layer = convnet.Layers(i);
   fprintf('%d make %s',i, class(layer));
   
   switch class(layer)
    case 'nnet.cnn.layer.ImageInputLayer'
       rows = layer.InputSize(1);
       cols = layer.InputSize(2);
       channels = layer.InputSize(3);
       
       addInitialCopyFromInput(HeaderFile,BodyFile,rows,cols,channels);

       addImageInputLayer(HeaderFile,BodyFile,level,layer,subtractAvg);
       level = level + 1;
       
       fprintf(' [done]');
       
    case 'nnet.cnn.layer.Convolution2DLayer'
       [rows, cols, channels] = addConvolution2Dlayer(HeaderFile,BodyFile,level,layer,rows,cols);
       level = level + 1;
       
       fprintf(' [done]');
    case 'nnet.cnn.layer.ReLULayer'
       addReLULayer(HeaderFile,BodyFile,level,rows,cols,channels);
       level = level + 1;
       
       fprintf(' [done]');
    case 'nnet.cnn.layer.LeakyReLULayer'
       addLeakyReLULayer(HeaderFile,BodyFile,level,layer,rows,cols,channels);
       level = level + 1;
       
       fprintf(' [done]');
    case 'nnet.cnn.layer.MaxPooling2DLayer'
       [rows, cols, channels] = addMaxPolling2dLayer(HeaderFile,BodyFile,level,layer,rows,cols,channels);
       level = level + 1;
       
       fprintf(' [done]');
    case 'nnet.cnn.layer.FullyConnectedLayer'
       [rows,cols, channels] = addFullyConnectedLayer(HeaderFile,BodyFile,level,layer,rows,cols,channels);
       level = level + 1;
       
       fprintf(' [done]');
    case 'nnet.cnn.layer.BatchNormalizationLayer'
       addBatchNormalizationLayer(HeaderFile,BodyFile,level,layer,rows,cols,channels);
       level = level + 1;
       
       fprintf(' [done]');
    case 'nnet.cnn.layer.SoftmaxLayer'
       addSoftMaxLayer(HeaderFile,BodyFile,level,rows,cols,channels);
       level = level + 1;
       
       fprintf(' [done]');
    case 'nnet.cnn.layer.ClassificationOutputLayer'
       addClassificationOutputLayer(HeaderFile,BodyFile,level,rows,cols,channels,name);
       
       fprintf(' [in development]');
    otherwise
       fprintf(2,'[skip unknown layer] ');
   end
   
   fprintf('\n');
end

fprintf(HeaderFile,'};\n\n'); % close class
fprintf(HeaderFile,'#endif\n\n'); % end ifdef

fprintf(BodyFile,'}\n\n'); % close function definition of classify

fclose(HeaderFile);
fclose(BodyFile);

end
