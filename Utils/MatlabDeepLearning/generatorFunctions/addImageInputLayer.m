function [ output_args ] = addImageInputLayer(fileID,step,layer)
%ADDIMAGEINPUTLAYER Summary of this function goes here
%   Detailed explanation goes here

% HACK: we need to access a private member 'PrivateLayer' in order to get
% the average image
warning off MATLAB:structOnObject
averageImage = struct(layer).PrivateLayer.AverageImage;

rows = layer.InputSize(1);
cols = layer.InputSize(2);

fprintf(fileID, '// declare output for this image input step\n');
fprintf(fileID, 'double out_step%d[%d][%d];\n\n', step, rows, cols);

fprintf(fileID, '// performe zero mean normalization\n');
for x = 1:rows
        for y = 1:cols
            fprintf(fileID, 'out_step%d[%d][%d] = out_step%d[%d][%d] - %0.5e;\n', step, x-1, y-1, step-1, x-1, y-1, averageImage(x,y));
        end
end

end

