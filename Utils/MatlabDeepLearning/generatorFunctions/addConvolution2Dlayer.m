function [out_dim_x, out_dim_y, out_dim_z] = addConvolution2Dlayer(HeaderFile,BodyFile, step, layer,in_dim_x,in_dim_y)

    weights = layer.Weights;
    bias    = layer.Bias;

    stride_x  = layer.Stride(1);
    stride_y  = layer.Stride(2);
    padding_x = layer.PaddingSize(1);
    padding_y = layer.PaddingSize(2);
    
    filter_dim_x = layer.FilterSize(1);
    filter_dim_y = layer.FilterSize(2);
    channels = layer.NumChannels;

    out_dim_x = floor((in_dim_x - filter_dim_x + 2*padding_x)/stride_x + 1);
    out_dim_y = floor((in_dim_y - filter_dim_y + 2*padding_y)/stride_y + 1);
    out_dim_z = size(weights,4);
    
    fprintf(HeaderFile, '\t// declare output for this convolution step\n');
    fprintf(HeaderFile, '\tfloat out_step%d[%d][%d][%d];\n\n', step, out_dim_x, out_dim_y, out_dim_z);
    
    fprintf(BodyFile, '// determine output for this convolution step\n');
    
    for z_out = 1:out_dim_z
        b = bias(1,1,z_out);
        w = weights(:,:,:,z_out);
        
        x_out = 0;
        for x = -padding_x + 1 : stride_x : in_dim_x + padding_x - filter_dim_x + 1
            y_out = 0;
            for y = -padding_y + 1 : stride_y : in_dim_y + padding_y - filter_dim_y + 1

                fprintf(BodyFile, 'out_step%d[%d][%d][%d] = ', step, x_out, y_out, z_out-1);
                
                for c = 1:channels
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
                                    fprintf(BodyFile, '\n  ');
                                end

                                if w(f_x,f_y,c) >= 0
                                    fprintf(BodyFile, ' + ');
                                else
                                    fprintf(BodyFile, ' - ');
                                end

                               fprintf(BodyFile, '%.8ff * out_step%d[%2d][%2d][%d]', abs(w(f_x,f_y,c)), step-1, (x+f_x-1)-1, (y+f_y-1)-1, c-1);

                               i = i+1;
                           end
                        end % f_y
                    end % f_x
                end % channels

                fprintf(BodyFile, '\n  ');
                if b >= 0
                    fprintf(BodyFile, ' + ');
                else
                    fprintf(BodyFile, ' - ');
                end

                fprintf(BodyFile, '%.8ff;\n\n', abs(b));

                y_out = y_out + 1;
            end % y
            x_out = x_out + 1;
        end % x
    end % z_out
    
    fprintf(BodyFile,'\n');
    
    %{
    fprintf(BodyFile,'out << "\\n\\n";\n');
    fprintf(BodyFile,'out << "out_step%d = zeros(7,7,8);";\n', step);
    fprintf(BodyFile,'for (int c = 0; c < 8; c++){\n');
    fprintf(BodyFile,'  out << "\\n\\nout_step%d(:,:," << (c+1) << ") = [";\n', step);
    fprintf(BodyFile,'  for (int i = 0; i < 7; i++){\n');
    fprintf(BodyFile,'    out << (i>0 ? "\\n" : "");\n');
    fprintf(BodyFile,'    for (int j = 0; j < 7; j++){\n');
    fprintf(BodyFile,'      out << (j>0 ? "," : "") << out_step%d[i][j][c];\n', step);
    fprintf(BodyFile,'    }\n');
    fprintf(BodyFile,'  }\n');
    fprintf(BodyFile,'  out << "];";\n');
    fprintf(BodyFile,'}\n');
    %}
end

