function [] = addReLULayer(fileID,step,rows,cols)
%ADDIMAGEINPUTLAYER Summary of this function goes here
%   Detailed explanation goes here

fprintf(fileID, '// declare output for this ReLU step\n');
fprintf(fileID, 'double out_step%d[%d][%d];\n\n', step, rows, cols);

fprintf(fileID, '// determine value of activation function Rectified-Linear-Unit\n');
for x = 1:rows
        for y = 1:cols
            fprintf(fileID, 'out_step%d[%d][%d] = (out_step%d[%d][%d] > 0) ? out_step%d[%d][%d] : 0;\n', step, x-1, y-1, step-1, x-1, y-1,  step-1, x-1, y-1);
        end
end

end