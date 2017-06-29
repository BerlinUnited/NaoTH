function [out_dim_x, out_dim_y] = addConvolution2Dlayer(fileID, step,layer,in_dim_x,in_dim_y)
%ADD Summary of this function goes here
%   Detailed explanation goes here    
    stride_x  = layer.Stride(1);
    stride_y  = layer.Stride(2);
    padding_x = layer.Padding(1);
    padding_y = layer.Padding(2);
    
    filter_dim_x = layer.FilterSize(1);
    filter_dim_y = layer.FilterSize(2);
    bias     = layer.Bias;
    weights  = layer.Weights;

    out_dim_x = floor((in_dim_x - filter_dim_x + 2*padding_x)/stride_x + 1);
    out_dim_y = floor((in_dim_y - filter_dim_y + 2*padding_y)/stride_y + 1);
    fprintf(fileID, '// declare output for this convolution step\n');
    fprintf(fileID, 'double out_step%d[%d][%d];\n\n', step, out_dim_x, out_dim_y);
    
    fprintf(fileID, '// determine output for this convolution step\n');
    x_out = 0;
    
    for x = -padding_x + 1 : stride_x : in_dim_x + padding_x - filter_dim_x + 1
        y_out = 0;
        for y = -padding_y + 1 : stride_y : in_dim_y + padding_y - filter_dim_y + 1
            
            fprintf(fileID, 'out_step%d[%d][%d] = ', step, x_out, y_out);
            i = 0;
            % 1 > x,y cords > in_dim
            % mat to c cords :  -1
            for f_x = 1:filter_dim_x
                for f_y = 1:filter_dim_y
                   if (x+f_x-1 < 1 || x+f_x-1 > in_dim_x) || (y+f_y-1 < 1 || y+f_y-1 > in_dim_y) 
                       continue;
                   else
                       % make the file more readable
                        if mod(i,filter_dim_x) == 0
                            fprintf(fileID, '\n  ');
                        end

                        if i > 0 && weights(f_x,f_y) >= 0
                            fprintf(fileID, ' + ');
                        else
                            fprintf(fileID, ' - ');
                        end

                       fprintf(fileID, '%.8f * out_step%d[%2d][%2d]', abs(weights(f_x,f_y)), step-1, x+f_x-1-1, y+f_y-1-1);
                       %cstring(i)   = string(sprintf('%d * out_step%d[%d][%d]', weights(f_x,f_y), step-1, x+f_x-1-1, y+f_y-1-1));
                       %cstring(i+1) = " + ";
                       %i = i+2;
                       i = i+1;
                   end
                end
            end
            
            % 
            %fprintf(fileID, 'out_step%d[%d][%d] = (%s) + %0.5e;\n', step, x_out, y_out, strjoin(cstring(1:numel(cstring)-1)), bias);

            fprintf(fileID, '\n  ');
            if bias >= 0
                fprintf(fileID, ' + ');
            else
                fprintf(fileID, ' - ');
            end

            fprintf(fileID, '%.8f;\n\n', abs(bias));
            
            y_out = y_out + 1;
        end
        x_out = x_out + 1;
    end
end

