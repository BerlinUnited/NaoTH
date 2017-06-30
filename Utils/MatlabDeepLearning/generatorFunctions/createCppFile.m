function [path] = createCppFile(convnet, file)

BodyFile   = fopen(strcat(file,'.cpp'), 'w');
HeaderFile = fopen(strcat(file,'.h'), 'w');

[~,name,~] = fileparts(file);

fprintf(HeaderFile,strcat('#ifndef _',name,'_H_\n'));
fprintf(HeaderFile,strcat('#define _',name,'_H_\n\n'));

fprintf(HeaderFile,'#include <limits>\n\n');

fprintf(HeaderFile,'#include "Representations/Perception/BallCandidates.h"\n\n');

fprintf(HeaderFile,'class %s{\n\n',name);
fprintf(HeaderFile,'public:\n');
fprintf(HeaderFile,'\t%s(){}\n\n',name);

%fprintf(HeaderFile,'\tbool classify( double* /*BallCandidates::Patch&*/ p/*, naoth::CameraInfo::CameraID cameraId*/);\n\n');
fprintf(HeaderFile,'\tbool classify(const BallCandidates::Patch& p);\n\n');

fprintf(BodyFile,strcat('#include "',name,'.h"\n\n'));
%fprintf(BodyFile,'bool %s::classify( double* /*BallCandidates::Patch&*/ p/*, naoth::CameraInfo::CameraID cameraId*/){\n',name);
fprintf(BodyFile,'bool %s::classify(const BallCandidates::Patch& p){\n',name);

layers = convnet.Layers;
for i = 1:numel(layers)
   fprintf('%d make %s',i, class(layers(i)));
   
   switch class(layers(i))
    case 'nnet.cnn.layer.ImageInputLayer'
       rows = layers(i).InputSize(1);
       cols = layers(i).InputSize(2);

       addInitialCopyFromInput(HeaderFile,BodyFile,rows,cols);

       addImageInputLayer(HeaderFile,BodyFile,i,layers(i));
       
       fprintf(' [done]');
    case 'nnet.cnn.layer.Convolution2DLayer'
       [rows, cols] = addConvolution2Dlayer(HeaderFile,BodyFile,i,layers(i),rows,cols);
       fprintf(' [done]');
    case 'nnet.cnn.layer.ReLULayer'
       addReLULayer(HeaderFile,BodyFile,i,rows,cols);
       fprintf(' [done]');
    case 'nnet.cnn.layer.MaxPooling2DLayer'
       %dummy();
       fprintf(' [missing]');
    case 'nnet.cnn.layer.FullyConnectedLayer'
       [rows,cols] = addFullyConnectedLayer(HeaderFile,BodyFile,i,layers(i),rows,cols);
       fprintf(' [done]');
    case 'nnet.cnn.layer.SoftmaxLayer'
       addSoftMaxLayer(HeaderFile,BodyFile,i,rows,cols);
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

end
