function [] = addSoftMaxLayer(fileID,step,rows,cols)
%ADDSOFTMAXLAYER Summary of this function goes here
%   Detailed explanation goes here

fprintf(fileID, '// declare output for this SoftMax step\n');
fprintf(fileID, 'double out_step%d[%d][%d];\n\n', step, rows, cols);

fprintf(fileID, '// determine maximum\n');
fprintf(fileID, 'double x_max = std::numeric_limits<double>::min();\n');
fprintf(fileID, 'for ( int i = 0; i < %d; i++){\n',rows);
fprintf(fileID, '\tfor ( int j = 0; j < %d; j++){\n',cols);
fprintf(fileID, '\t\tif (out_step%d[i][j] > x_max){\n',step-1);
fprintf(fileID, '\t\t\tx_max = out_step%d[i][j];\n',step-1);
fprintf(fileID, '\t\t}\n');
fprintf(fileID, '\t}\n');
fprintf(fileID, '}\n\n');

fprintf(fileID, '// subtract max, use as exponent and sum up\n');
fprintf(fileID, 'double sum = 0;\n');
fprintf(fileID, 'for ( int i = 0; i < %d; i++){\n',rows);
fprintf(fileID, '\tfor ( int j = 0; j < %d; j++){\n',cols);
fprintf(fileID, '\t\tout_step%d[i][j] = exp(out_step%d[i][j] - x_max)\n',step,step-1);
fprintf(fileID, '\t\tsum += out_step%d[i][j] \n',step);
fprintf(fileID, '\t}\n');
fprintf(fileID, '}\n\n');

fprintf(fileID, '// normalize\n');
fprintf(fileID, 'for ( int i = 0; i < %d; i++){\n',rows);
fprintf(fileID, '\tfor ( int j = 0; j < %d; j++){\n',cols);
fprintf(fileID, '\t\tout_step%d[i][j] /= sum\n',step);
fprintf(fileID, '\t}\n');
fprintf(fileID, '}\n');
end

