function [ output_args ] = addInitialCopyFromInput(HeaderFile,BodyFile,rows,cols)
%ADDINITIALCOPYFROMINPUT Summary of this function goes here
%   Detailed explanation goes here


fprintf(HeaderFile, '\t// declare copied intial input array\n');
fprintf(HeaderFile, '\tdouble out_step%d[%d][%d];\n\n', 0, rows, cols);

fprintf(BodyFile, '// copy patch into intial data array out_step0\n');
fprintf(BodyFile,'ASSERT(p.data.size() == %d);\n\n',rows*cols);

for x = 1:rows
    for y = 1:cols
        fprintf(BodyFile, 'out_step%d[%2d][%2d] = p.data[%2d];\n', 0, x-1, y-1,cols*(y-1) + (x-1));
    end
end

fprintf(BodyFile,'\n');

end

