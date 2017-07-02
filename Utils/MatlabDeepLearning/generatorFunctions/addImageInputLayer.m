function addImageInputLayer(HeaderFile,BodyFile,step,layer)

% HACK: we need to access a private member 'PrivateLayer' in order to get
% the average image
warning off MATLAB:structOnObject
averageImage = struct(layer).PrivateLayer.AverageImage;

rows = layer.InputSize(1);
cols = layer.InputSize(2);
channels = layer.InputSize(3);

fprintf(HeaderFile, '\t// declare output for this image input step\n');
fprintf(HeaderFile, '\tdouble out_step%d[%d][%d][%d];\n\n', step, rows, cols, channels);

fprintf(BodyFile, '// perform zero mean normalization\n');
for c = 1:channels
    for x = 1:rows
        for y = 1:cols
            fprintf(BodyFile, 'out_step%d[%2d][%2d][%d] = out_step%d[%2d][%2d][%d] - %.8f;\n', step, x-1, y-1, c-1, step-1, x-1, y-1, c-1, averageImage(x,y,c));
        end
    end
end

fprintf(BodyFile,'\n');

end

