function [ output_args ] = addImageInputLayer(HeaderFile,BodyFile,step,layer)
%ADDIMAGEINPUTLAYER Summary of this function goes here
%   Detailed explanation goes here

% HACK: we need to access a private member 'PrivateLayer' in order to get
% the average image
warning off MATLAB:structOnObject
averageImage = struct(layer).PrivateLayer.AverageImage;

rows = layer.InputSize(1);
cols = layer.InputSize(2);

fprintf(HeaderFile, '\t// declare output for this image input step\n');
fprintf(HeaderFile, '\tdouble out_step%d[%d][%d];\n\n', step, rows, cols);

fprintf(BodyFile, '// performe zero mean normalization\n');
for x = 1:rows
    for y = 1:cols
        fprintf(BodyFile, 'out_step%d[%2d][%2d] = out_step%d[%2d][%2d] - %.8f;\n', step, x-1, y-1, step-1, x-1, y-1, averageImage(x,y));
    end
end

fprintf(BodyFile,'\n');

end

