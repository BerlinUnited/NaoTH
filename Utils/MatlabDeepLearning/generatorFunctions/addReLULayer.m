function [] = addReLULayer(HeaderFile,BodyFile,step,rows,cols)
%ADDIMAGEINPUTLAYER Summary of this function goes here
%   Detailed explanation goes here

fprintf(HeaderFile, '\t// declare output for this ReLU step\n');
fprintf(HeaderFile, '\tdouble out_step%d[%d][%d];\n\n', step, rows, cols);

fprintf(BodyFile, '// determine value of activation function Rectified-Linear-Unit\n');
for x = 1:rows
    for y = 1:cols
        fprintf(BodyFile, 'out_step%d[%2d][%2d] = (out_step%d[%2d][%2d] > 0) ? out_step%d[%2d][%2d] : 0;\n', step, x-1, y-1, step-1, x-1, y-1,  step-1, x-1, y-1);
    end
end

fprintf(BodyFile,'\n');

end