function [ out_rows, out_cols ] = addFullyConnectedLayer( HeaderFile,BodyFile,step,layer,rows,cols )
%ADDFULLYCONNECTEDLAYER Summary of this function goes here
%   Detailed explanation goes here
out_rows = layer.OutputSize;
out_cols = 1;

bias     = layer.Bias;
weights  = layer.Weights; %reshape(layer.Weights,out_rows,rows,cols);

fprintf(HeaderFile, '\t// declare output for this fully connected step\n');
fprintf(HeaderFile, '\tdouble out_step%d[%d][%d];\n\n', step, out_rows, 1);

fprintf(BodyFile, '// determine output for this fully connected step\n');

i = 0;
for out = 1:out_rows % 
    
    fprintf(BodyFile, 'out_step%d[%d][%d] = ', step, out-1, 0);
    % iterate over input
    for x = 1 : rows
        for y = 1 : cols
            idx = 1+(x-1)*cols + (y-1);
            
            % make the file more readable
            if mod(i,4) == 0
                fprintf(BodyFile, '\n  ');
            end
            
            if i > 0 && weights(out,idx) >= 0
                fprintf(BodyFile, ' + ');
            else
                fprintf(BodyFile, ' - ');
            end
            
            fprintf(BodyFile, '%.8f * out_step%d[%2d][%2d]', abs(weights(out,idx)), step-1, y-1, x-1);
            
            i = i+1;
        end
    end
    
    fprintf(BodyFile, '\n  ');
    if bias(out) >= 0
        fprintf(BodyFile, ' + ');
    else
        fprintf(BodyFile, ' - ');
    end
    
    fprintf(BodyFile, '%.8f;\n\n', abs(bias(out)));
end

fprintf(BodyFile,'\n');

end


