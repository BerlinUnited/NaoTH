function addImageInputLayer(HeaderFile,BodyFile,step,layer,subtractAvg)

% HACK: we need to access a private member 'PrivateLayer' in order to get
% the average image
warning off MATLAB:structOnObject
averageImage = struct(layer).PrivateLayer.AverageImage;

rows = layer.InputSize(1);
cols = layer.InputSize(2);
channels = layer.InputSize(3);

fprintf(HeaderFile, '\t// declare output for this image input step\n');
fprintf(HeaderFile, '\tfloat out_step%d[%d][%d][%d];\n\n', step, rows, cols, channels);

fprintf(BodyFile, '// perform zero mean normalization\n');
for c = 1:channels
    for x = 1:rows
        for y = 1:cols
            if subtractAvg
                % with average image
                fprintf(BodyFile, 'out_step%d[%2d][%2d][%d] = out_step%d[%2d][%2d][%d] - %.8ff;\n', step, x-1, y-1, c-1, step-1, x-1, y-1, c-1, averageImage(x,y,c));
            else
                % without average image
                fprintf(BodyFile, 'out_step%d[%2d][%2d][%d] = out_step%d[%2d][%2d][%d];\n', step, x-1, y-1, c-1, step-1, x-1, y-1, c-1);
            end
            end
    end
end

% test output
%{
fprintf(BodyFile,'out << "\\n\\n";\n');
fprintf(BodyFile, 'out << "img = [";\n');
fprintf(BodyFile, 'for ( int i = 0; i < %d; i++){\n',rows);
fprintf(BodyFile, '\tout << (i>0?"\\n":"");\n');
fprintf(BodyFile, '\tfor ( int j = 0; j < %d; j++){\n',cols);
fprintf(BodyFile, '\t\tout << (j>0?",":"") << out_step%d[i][j][0]; \n',step-1);
fprintf(BodyFile, '\t}\n');
fprintf(BodyFile, '}\n');
fprintf(BodyFile, 'out << "];";\n');
%}
fprintf(BodyFile,'\n');

end

