function [] = addSoftMaxLayer(HeaderFile,BodyFile,step,rows,cols)
%ADDSOFTMAXLAYER Summary of this function goes here
%   Detailed explanation goes here

fprintf(HeaderFile, '\t// declare output for this SoftMax step\n');
fprintf(HeaderFile, '\tdouble out_step%d[%d][%d];\n\n', step, rows, cols);

fprintf(BodyFile, '// determine maximum\n');
fprintf(BodyFile, 'double x_max = std::numeric_limits<double>::min();\n');
fprintf(BodyFile, 'for ( int i = 0; i < %d; i++){\n',rows);
fprintf(BodyFile, '\tfor ( int j = 0; j < %d; j++){\n',cols);
fprintf(BodyFile, '\t\tif (out_step%d[i][j] > x_max){\n',step-1);
fprintf(BodyFile, '\t\t\tx_max = out_step%d[i][j];\n',step-1);
fprintf(BodyFile, '\t\t}\n');
fprintf(BodyFile, '\t}\n');
fprintf(BodyFile, '}\n\n');

fprintf(BodyFile, '// subtract max, use as exponent and sum up\n');
fprintf(BodyFile, 'double sum = 0;\n');
fprintf(BodyFile, 'for ( int i = 0; i < %d; i++){\n',rows);
fprintf(BodyFile, '\tfor ( int j = 0; j < %d; j++){\n',cols);
fprintf(BodyFile, '\t\tout_step%d[i][j] = exp(out_step%d[i][j] - x_max);\n',step,step-1);
fprintf(BodyFile, '\t\tsum += out_step%d[i][j]; \n',step);
fprintf(BodyFile, '\t}\n');
fprintf(BodyFile, '}\n\n');

fprintf(BodyFile, '// normalize\n');
fprintf(BodyFile, 'for ( int i = 0; i < %d; i++){\n',rows);
fprintf(BodyFile, '\tfor ( int j = 0; j < %d; j++){\n',cols);
fprintf(BodyFile, '\t\tout_step%d[i][j] /= sum;\n',step);
fprintf(BodyFile, '\t}\n');
fprintf(BodyFile, '}\n');

fprintf(BodyFile,'\n');
end

