function [ out_rows, out_cols, out_channels ] = addFullyConnectedLayer( HeaderFile,BodyFile,step,layer,rows,cols,channels )

out_rows = layer.OutputSize;
out_cols = 1;
out_channels = 1;

bias     = layer.Bias;
weights  = layer.Weights; %reshape(layer.Weights,out_rows,rows,cols);

fprintf(HeaderFile, '\t// declare output for this fully connected step\n');
fprintf(HeaderFile, '\tfloat out_step%d[%d][%d][%d];\n\n', step, out_rows, out_cols, out_channels);

fprintf(BodyFile, '// determine output for this fully connected step\n');

i = 0;
for out = 1:out_rows % 
    
    % hack: there is allways only 1 out channel
    fprintf(BodyFile, 'out_step%d[%d][%d][%d] = ', step, out-1, 0, 0);
    
    % iterate over input
    for c = 1 : channels
        for x = 1 : rows
            for y = 1 : cols
                idx = (c-1)*(cols*rows) + (y-1)*cols + x;

                % make the file more readable
                if mod(i,4) == 0
                    fprintf(BodyFile, '\n  ');
                end

                if weights(out,idx) >= 0
                    fprintf(BodyFile, ' + ');
                else
                    fprintf(BodyFile, ' - ');
                end
                fprintf(BodyFile, '%.8ff * out_step%d[%2d][%2d][%d]', abs(weights(out,idx)), step-1, x-1, y-1, c-1);
                i = i+1;
            end
        end
    end
    
    fprintf(BodyFile, '\n  ');
    if bias(out) >= 0
        fprintf(BodyFile, ' + ');
    else
        fprintf(BodyFile, ' - ');
    end
    
    fprintf(BodyFile, '%.8ff;\n\n', abs(bias(out)));
end

fprintf(BodyFile,'\n');

end


