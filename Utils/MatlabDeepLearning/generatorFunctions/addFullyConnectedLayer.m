function [ out_rows, out_cols ] = addFullyConnectedLayer( fileID,step,layer,rows,cols )
%ADDFULLYCONNECTEDLAYER Summary of this function goes here
%   Detailed explanation goes here
out_rows = layer.OutputSize;
out_cols = 1;

bias     = layer.Bias;
weights  = layer.Weights; %reshape(layer.Weights,out_rows,rows,cols);

fprintf(fileID, '// declare output for this fully connected step\n');
fprintf(fileID, 'double out_step%d[%d][%d];\n\n', step, out_rows, 1);

fprintf(fileID, '// determine output for this fully connected step\n');

for out = 1:out_rows % 
    
    fprintf(fileID, 'out_step%d[%d][%d] = ', step, out-1, 0);
    % iterate over input
    for x = 1 : rows
        for y = 1 : cols
            idx = 1+(x-1)*cols + (y-1);
            fprintf(fileID, '%d * out_step%d[%d][%d] + ', weights(out,idx), step-1, y-1, x-1);
        end
    end
    
    fprintf(fileID, '%d;', bias(out));
end
end


