function [out_dim_x, out_dim_y, out_dim_z] = addMaxPolling2dLayer(HeaderFile, BodyFile, step, layer, in_dim_x, in_dim_y, channels)

    stride_x  = layer.Stride(1);
    stride_y  = layer.Stride(2);
    padding_x = layer.Padding(1);
    padding_y = layer.Padding(2);
    pool_x = layer.PoolSize(1);
    pool_y = layer.PoolSize(2);
    
    out_dim_x = floor((in_dim_x - filter_dim_x + 2*padding_x)/stride_x + 1);
    out_dim_y = floor((in_dim_y - filter_dim_y + 2*padding_y)/stride_y + 1);
    %out_dim_z = size(weights,4);
    
    fprintf(HeaderFile, '\t// declare output for this max polling step\n');
    fprintf(HeaderFile, '\tdouble out_step%d[%d][%d][%d];\n\n', step, out_dim_x, out_dim_y, out_dim_z);
    
    fprintf(BodyFile, '// determine output for this max polling step\n');
    
    for z_out = 1:out_dim_z        
        x_out = 0;
        for x = -padding_x + 1 : stride_x : in_dim_x + padding_x - filter_dim_x + 1
            y_out = 0;
            for y = -padding_y + 1 : stride_y : in_dim_y + padding_y - filter_dim_y + 1

                fprintf(BodyFile, 'out_step%d[%d][%d][%d] = ', step, x_out, y_out, z_out-1);
                
                for c = 1:channels
                    i = 0;
                    % 1 > x,y cords > in_dim
                    % mat to c cords :  -1
                    fprintf(BodyFile, 'out_step%d[%2d][%2d][%d] = out_step%d[%2d][%2d][%d]' ,step, x-1, y-1, c-1, step-1, x-1, y-1, c-1);
  
                    for f_x = 1:pool_x
                        for f_y = 1:pool_y
                           if (x+f_x-1 < 1 || x+f_x-1 > in_dim_x) || (y+f_y-1 < 1 || y+f_y-1 > in_dim_y) 
                               continue;
                           else
                               fprintf(BodyFile, 'if(out_step%d[%2d][%2d][%d] > out_step%d[%2d][%2d][%d]) out_step%d[%2d][%2d][%d] = out_step%d[%2d][%2d][%d];'...
                                               ,     step, x-1, y-1, c-1,       step-1, (x+f_x-1)-1, (y+f_y-1)-1, c-1,     step, x-1, y-1, c-1,      step-1, (x+f_x-1)-1, (x+f_y-1)-1, c-1);
                               i = i+1;
                           end
                        end % f_y
                    end % f_x
                end % channels

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

