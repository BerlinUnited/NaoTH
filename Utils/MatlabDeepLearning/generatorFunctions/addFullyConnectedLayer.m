function [ out_rows, out_cols ] = addFullyConnectedLayer( fileID,step,layer,rows,cols )
%ADDFULLYCONNECTEDLAYER Summary of this function goes here
%   Detailed explanation goes here
out_rows = layer.OutputSize;
out_cols = 1;

bias     = layer.Bias;
weights  = reshape(layer.Weights,out_rows,rows,cols);

fprintf(fileID, '// declare output for this fully connected step\n');
fprintf(fileID, 'double out_step%d[%d][%d];\n\n', step, out_rows, 1);

fprintf(fileID, '// determine output for this fully connected step\n');

i = 1;
for out = 1:out_rows
    for x = 1 : rows
        for y = 1 : cols
            cstring(i)   = string(sprintf('%d * out_step%d[%d][%d]', weights(out,x,y), step-1, x-1, y-1));
            cstring(i+1) = " + ";
            i = i+2;
        end
    end
    
    fprintf(fileID, 'out_step%d[%d][%d] = (%s) + %0.5e;\n', step, out-1, 0, strjoin(cstring(1:numel(cstring)-1)), bias(out));
end
end

