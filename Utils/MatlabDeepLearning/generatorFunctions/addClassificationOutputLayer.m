function [ output_args ] = addClassificationOutputLayer(HeaderFile,BodyFile,step,rows,cols,channels,name)
%ADDCLASSIFICATIONOUTPUTLAYER Summary of this function goes here
%   Detailed explanation goes here

fprintf(BodyFile,'// return classification\n');
% if rows > 1
%     fprintf(BodyFile, 'return out_step%d[0][0][0] > out_step%d[1][0][0];\n' ,step-1, step-1);
% else
%     fprintf(BodyFile, 'return out_step%d[0][0][0] > out_step%d[0][0][1];\n', step-1, step-1);
% end
fprintf(BodyFile,'return getBallConfidence() > getNoballConfidence();\n');
fprintf(BodyFile,'}\n\n'); % close function definition of classify    

fprintf(HeaderFile,'public:\n');
fprintf(HeaderFile,'\tvirtual float getBallConfidence();\n\n');
fprintf(HeaderFile,'\tvirtual float getNoballConfidence();\n\n');

fprintf(BodyFile,'float %s::getBallConfidence(){\n', name);
fprintf(BodyFile,'\treturn out_step%d[0][0][0];\n', step-1);
fprintf(BodyFile,'}\n\n'); % close function definition of getBallConfidence    

fprintf(BodyFile,'float %s::getNoballConfidence(){\n', name);
if rows > 1
    fprintf(BodyFile,'\treturn out_step%d[1][0][0];\n', step-1);
else
    fprintf(BodyFile,'\treturn out_step%d[0][0][1];\n', step-1);
end
%function definition wiil be closed at the end of createCppFile.m


