clear variables
clc
%% init defaults
warning('off', 'MATLAB:MKDIR:DirectoryExists');
addpath('../common')
secret()
try
    load(reference_database_path)
    disp('Loading the reference database')
catch
    disp('ERROR: No reference database was found')
    return;
end
%% Get the reference categories
ref_categories = fieldnames(reference_database);

for k=1:length(ref_categories)
    current_categorie = char(ref_categories(k));
    current_categorie_struct = reference_database.(current_categorie);
    
    for i=1:length(current_categorie_struct)    
        % Export the recordings as raw in the correct folder structure
        name = current_categorie_struct(i).name;
        rawData = current_categorie_struct(i).rawData;

        path_to_outputfolder = fullfile(reference_export_path, current_categorie);
        mkdir(path_to_outputfolder)

        filename = fullfile(path_to_outputfolder, strcat(name, '.raw'));

        % Export raw data
        fileID = fopen(filename, 'w');
            fwrite(fileID, rawData, 'int16', 'ieee-le');
        fclose(fileID);

        % Export wav
        filename2 = fullfile(path_to_outputfolder, strcat(name, '.wav'));
        Fs = current_categorie_struct(i).samplerate;
        y = rawData ./ max( abs(rawData(:)) );
        audiowrite(char(filename2),y,Fs);

        % Export the spectrum as dat
        filename3 = fullfile(path_to_outputfolder, strcat(name, '.dat'));
        fileID = fopen (filename3, 'w');
            % unpack complex array again
            real_part = real(current_categorie_struct(i).spectralData);
            imag_part = imag(current_categorie_struct(i).spectralData);
            % interleave the real and imaginary part for output
            output = interleave(real_part, imag_part);
            % append max autocorrelation to the output
            output = [output current_categorie_struct(i).autocorrelation];
            fwrite(fileID, output, 'double', 'ieee-le');
        fclose(fileID);
    end
end