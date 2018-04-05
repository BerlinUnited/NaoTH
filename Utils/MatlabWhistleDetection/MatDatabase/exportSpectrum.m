clear variables
clc
%% Check if reference_database exists
try
    load('reference_database.mat')
    disp('INFO: using a previously created database')
catch
    disp('INFO: no reference_database was found')
    return
end
% get categories
names = string(fieldnames(reference_database));

for i=1:length(names)
    category = reference_database.(names(i));
    
    % create folders for the output
    % TODO putput should be moved to the data folder
    [status, msg, msgID] = mkdir(names(i));
    
    % iterate over every reference whistle in this category
    for j=1:length(category)
        %TODO save spectrum in correct subfolders
        output_name = strcat(category(j).name, '.dat');
        output_name = fullfile(names(i), output_name);
        fileID = fopen (output_name, 'w');
            % unpack complex array again
            real_part = real(category(j).spectralData);
            imag_part = imag(category(j).spectralData);
            % interleave the real and imaginary part for output
            output = interleave(real_part, imag_part);
            % append max autocorrelation to the output
            output = [output category(j).autocorrelation];
            fwrite(fileID, output, 'double', 'ieee-le');            
        fclose(fileID);
    end
end
% input a category name
% input a path where output should be stored

% export audio files as wav

% output nothing
