function [ output_args ] = addClassificationOutputLayer(HeaderFile,BodyFile,step,rows,cols)
%ADDCLASSIFICATIONOUTPUTLAYER Summary of this function goes here
%   Detailed explanation goes here

fprintf(BodyFile, '// return classification\n');
fprintf(BodyFile, 'return out_step%d[%d][%d][0] > out_step%d[%d][%d][0];\n',step-1,0,0,step-1,1,0);

end

