function [path] = createCppFile(convnet, file)

BodyFile   = fopen(strcat(file,'.cpp'), 'w');
HeaderFile = fopen(strcat(file,'.h'), 'w');

[~,name,~] = fileparts(file);

fprintf(HeaderFile,strcat('#ifndef _',name,'_H_\n'));
fprintf(HeaderFile,strcat('#define _',name,'_H_\n\n'));

fprintf(HeaderFile,'#include <limits>\n\n');
fprintf(HeaderFile,'#include <fstream>\n\n');

fprintf(HeaderFile,'#include "Representations/Perception/BallCandidates.h"\n\n');

fprintf(HeaderFile,'class %s{\n\n',name);
fprintf(HeaderFile,'public:\n');
fprintf(HeaderFile,'\t%s(){out.open("example.txt");}\n\n',name);
fprintf(HeaderFile,'\t~%s(){out.close();}\n\n',name);
fprintf(HeaderFile,'std::ofstream out;\n');

%fprintf(HeaderFile,'\tbool classify( float* /*BallCandidates::Patch&*/ p/*, naoth::CameraInfo::CameraID cameraId*/);\n\n');
fprintf(HeaderFile,'\tbool classify(const BallCandidates::Patch& p);\n\n');

fprintf(BodyFile,strcat('#include "',name,'.h"\n\n'));
%fprintf(BodyFile,'bool %s::classify( float* /*BallCandidates::Patch&*/ p/*, naoth::CameraInfo::CameraID cameraId*/){\n',name);
fprintf(BodyFile,'bool %s::classify(const BallCandidates::Patch& p){\n',name);

layers = convnet.Layers;
for i = 1:numel(layers)
   
   layer = convnet.Layers(i);
   fprintf('%d make %s',i, class(layer));
   
   switch class(layer)
    case 'nnet.cnn.layer.ImageInputLayer'
       rows = layer.InputSize(1);
       cols = layer.InputSize(2);
       channels = layer.InputSize(3);
       
       addInitialCopyFromInput(HeaderFile,BodyFile,rows,cols,channels);

       addImageInputLayer(HeaderFile,BodyFile,i,layer);
       
       fprintf(' [done]');
       
    case 'nnet.cnn.layer.Convolution2DLayer'
       [rows, cols, channels] = addConvolution2Dlayer(HeaderFile,BodyFile,i,layer,rows,cols);
       fprintf(' [done]');
    case 'nnet.cnn.layer.ReLULayer'
       addReLULayer(HeaderFile,BodyFile,i,rows,cols,channels);
       fprintf(' [done]');
    case 'nnet.cnn.layer.MaxPooling2DLayer'
       [rows, cols, channels] = addMaxPolling2dLayer(HeaderFile,BodyFile,i,layer,rows,cols,channels);
       fprintf(' [done]');
    case 'nnet.cnn.layer.FullyConnectedLayer'
       [rows,cols, channels] = addFullyConnectedLayer(HeaderFile,BodyFile,i,layer,rows,cols,channels);
       fprintf(' [done]');
    case 'nnet.cnn.layer.SoftmaxLayer'
       addSoftMaxLayer(HeaderFile,BodyFile,i,rows,cols,channels);
       fprintf(' [done]');
    case 'nnet.cnn.layer.ClassificationOutputLayer'
       addClassificationOutputLayer(HeaderFile,BodyFile,i,rows,cols);
       fprintf(' [in development]');
    otherwise
   end
   
   fprintf('\n');
end

fprintf(HeaderFile,'};\n\n'); % close class
fprintf(HeaderFile,'#endif\n\n'); % end ifdef

fprintf(BodyFile,'}\n\n'); % close function definition of classify

fclose(HeaderFile);
fclose(BodyFile);

end
