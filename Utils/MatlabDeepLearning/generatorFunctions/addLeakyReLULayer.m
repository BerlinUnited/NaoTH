function [] = addLeakyReLULayer(HeaderFile,BodyFile,step,layer,rows,cols,channels)

fprintf(HeaderFile, '\t// declare output for this LeakyReLU step\n');
fprintf(HeaderFile, '\tfloat out_step%d[%d][%d][%d];\n\n', step, rows, cols, channels);

fprintf(BodyFile, '// determine value of activation function Rectified-Linear-Unit\n');
for c = 1:channels
    for x = 1:rows
        for y = 1:cols
            fprintf(BodyFile, 'out_step%d[%2d][%2d][%d] = (out_step%d[%2d][%2d][%d] > 0) ? out_step%d[%2d][%2d][%d] : %.8ff * out_step%d[%2d][%2d][%d];\n', step, x-1, y-1, c-1, step-1, x-1, y-1, c-1, step-1, x-1, y-1, c-1, layer.Scale, step-1, x-1, y-1, c-1);
        end
    end
end

fprintf(BodyFile,'\n');

