function data_augmentation( mat_file , varargin )
disp(mat_file)
[ data, labels ]= load_from_mat(mat_file);

for i=1:length(varargin)
    if(varargin{i} == 'brightness')
        [ data, labels ] = brightness(data, labels);
    end
end
%TODO indicate what was augmented in seperate variables
[filepath,name,ext] = fileparts(mat_file);
output = strcat(filepath,filesep,name, '-aug',ext);
disp(output)
save(output, 'data', 'labels');

end