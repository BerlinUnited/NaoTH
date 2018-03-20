function [] = addBatchNormalizationLayer(HeaderFile,BodyFile, step, layer,rows,cols,channels)

e = sqrt(layer.TrainedVariance + layer.Epsilon);
weights = layer.Scale./e;
bias = layer.Offset - layer.TrainedMean.*layer.Scale./e;

fprintf(HeaderFile, '\t// declare output for this ReLU step\n');
fprintf(HeaderFile, '\tfloat out_step%d[%d][%d][%d];\n\n', step, rows, cols, channels);

fprintf(BodyFile, '// determine value of activation function Rectified-Linear-Unit\n');
for c = 1:channels
    for x = 1:rows
        for y = 1:cols
            fprintf(BodyFile, 'out_step%d[%2d][%2d][%d] = out_step%d[%2d][%2d][%d] * %.8ff + %.8ff;\n', step, x-1, y-1, c-1, step-1, x-1, y-1, c-1, weights(c), bias(c));
        end
    end
end

fprintf(BodyFile,'\n');

end