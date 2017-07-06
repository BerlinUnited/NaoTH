function [] = addSoftMaxLayer(HeaderFile,BodyFile,step,rows,cols, channels)
%ADDSOFTMAXLAYER Summary of this function goes here
%   Detailed explanation goes here

fprintf(HeaderFile, '\t// declare output for this SoftMax step\n');
fprintf(HeaderFile, '\tfloat out_step%d[%d][%d][%d];\n\n', step, rows, cols, channels);

fprintf(BodyFile, '// determine maximum\n');
fprintf(BodyFile, 'float x_max = std::numeric_limits<float>::min();\n');
fprintf(BodyFile, 'for ( int c = 0; c < %d; c++){\n',channels);
fprintf(BodyFile, 'for ( int i = 0; i < %d; i++){\n',rows);
fprintf(BodyFile, '\tfor ( int j = 0; j < %d; j++){\n',cols);
fprintf(BodyFile, '\t\tif (out_step%d[i][j][c] > x_max){\n',step-1);
fprintf(BodyFile, '\t\t\tx_max = out_step%d[i][j][c];\n',step-1);
fprintf(BodyFile, '\t\t}\n');
fprintf(BodyFile, '\t}\n');
fprintf(BodyFile, '}\n');
fprintf(BodyFile, '}\n\n');

fprintf(BodyFile, '// subtract max, use as exponent and sum up\n');
fprintf(BodyFile, 'float sum = 0;\n');
fprintf(BodyFile, 'for ( int c = 0; c < %d; c++){\n',channels);
fprintf(BodyFile, 'for ( int i = 0; i < %d; i++){\n',rows);
fprintf(BodyFile, '\tfor ( int j = 0; j < %d; j++){\n',cols);
fprintf(BodyFile, '\t\tout_step%d[i][j][c] = std::exp(out_step%d[i][j][c] - x_max);\n',step,step-1);
fprintf(BodyFile, '\t\tsum += out_step%d[i][j][c]; \n',step);
fprintf(BodyFile, '\t}\n');
fprintf(BodyFile, '}\n');
fprintf(BodyFile, '}\n\n');

fprintf(BodyFile, '// normalize\n');
fprintf(BodyFile, 'for ( int c = 0; c < %d; c++){\n',channels);
fprintf(BodyFile, 'for ( int i = 0; i < %d; i++){\n',rows);
fprintf(BodyFile, '\tfor ( int j = 0; j < %d; j++){\n',cols);
fprintf(BodyFile, '\t\tout_step%d[i][j][c] /= sum;\n',step);
fprintf(BodyFile, '\t}\n');
fprintf(BodyFile, '}\n');
fprintf(BodyFile, '}\n\n');

% test output
%{
fprintf(BodyFile,'out << "\\n\\n";\n');
fprintf(BodyFile, 'out << "res = [";\n');
fprintf(BodyFile, 'for ( int i = 0; i < %d; i++){\n',rows);
fprintf(BodyFile, '\tout << (i>0?"\\n":"");\n');
fprintf(BodyFile, '\tfor ( int j = 0; j < %d; j++){\n',cols);
fprintf(BodyFile, '\t\tout << (j>0?",":"") << out_step%d[i][j][0]; \n',step);
fprintf(BodyFile, '\t}\n');
fprintf(BodyFile, '}\n');
fprintf(BodyFile, 'out << "];";\n');
fprintf(BodyFile,'\n');
%}
end

