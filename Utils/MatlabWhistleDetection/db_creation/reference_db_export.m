clear variables
clc
%% export reference whistles
database = 'reference_database.mat';
databasePath = 'D:\Downloads\WhistleData_mat';
load([databasePath '/' database])
%% Get the reference categories
ref_bhuman = reference_database.bhuman;
ref_kln17= reference_database.kln17;
ref_rc17= reference_database.rc17;

for i=1:length(ref_bhuman)    
    % Export the recordings as raw in the correct folder structure
    name = ref_bhuman(i).name;
    rawData = ref_bhuman(i).rawData;
    
    path_to_outputfolder = fullfile(databasePath, 'reference_data_raw/bhuman');
    mkdir(path_to_outputfolder)
    
    filename = fullfile(path_to_outputfolder, strcat(name, '.raw'));
    
    % Export raw data
    fileID = fopen(filename, 'w');
        fwrite(fileID, rawData, 'int16', 'ieee-le');
    fclose(fileID);
    
    % Export wav
    filename2 = fullfile(path_to_outputfolder, strcat(name, '.wav'));
    Fs = ref_bhuman(i).samplerate;
    y = rawData ./ max( abs(rawData(:)) );
    audiowrite(char(filename2),y,Fs);
    
    % Export the spectrum as dat
    filename3 = fullfile(path_to_outputfolder, strcat(name, '.dat'));
    fileID = fopen (filename3, 'w');
        % unpack complex array again
        real_part = real(ref_bhuman(i).spectralData);
        imag_part = imag(ref_bhuman(i).spectralData);
        % interleave the real and imaginary part for output
        output = interleave(real_part, imag_part);
        % append max autocorrelation to the output
        output = [output ref_bhuman(i).autocorrelation];
        fwrite(fileID, output, 'double', 'ieee-le');
    fclose(fileID);
end

for i=1:length(ref_kln17)
    % Export the recordings as raw in the correct folder structure
    name = ref_kln17(i).name;
    rawData = ref_kln17(i).rawData;
    
    path_to_outputfolder = fullfile(databasePath, 'reference_data_raw/kln17');
    mkdir(path_to_outputfolder)
    
    filename = fullfile(path_to_outputfolder, strcat(name, '.raw'));
    
    % Export raw data
    fileID = fopen(filename, 'w');
        fwrite(fileID, rawData, 'int16', 'ieee-le');
    fclose(fileID);
    
    % Export wav
    filename2 = fullfile(path_to_outputfolder, strcat(name, '.wav'));
    Fs = ref_kln17(i).samplerate;
    y = rawData ./ max( abs(rawData(:)) );
    audiowrite(char(filename2),y,Fs);
    
    % Export the spectrum as dat
    filename3 = fullfile(path_to_outputfolder, strcat(name, '.dat'));
    fileID = fopen (filename3, 'w');
        % unpack complex array again
        real_part = real(ref_kln17(i).spectralData);
        imag_part = imag(ref_kln17(i).spectralData);
        % interleave the real and imaginary part for output
        output = interleave(real_part, imag_part);
        % append max autocorrelation to the output
        output = [output ref_kln17(i).autocorrelation];
        fwrite(fileID, output, 'double', 'ieee-le');
    fclose(fileID);
end

for i=1:length(ref_rc17)
    % Export the recordings as raw in the correct folder structure
    name = ref_rc17(i).name;
    rawData = ref_rc17(i).rawData;
    
    path_to_outputfolder = fullfile(databasePath, 'reference_data_raw/rc17');
    mkdir(path_to_outputfolder)
    
    filename = fullfile(path_to_outputfolder, strcat(name, '.raw'));
    
    % Export raw data
    fileID = fopen(filename, 'w');
        fwrite(fileID, rawData, 'int16', 'ieee-le');
    fclose(fileID);
    
    % Export wav
    filename2 = fullfile(path_to_outputfolder, strcat(name, '.wav'));
    Fs = ref_rc17(i).samplerate;
    y = rawData ./ max( abs(rawData(:)) );
    audiowrite(char(filename2),y,Fs);
    
    % Export the spectrum as dat
    filename3 = fullfile(path_to_outputfolder, strcat(name, '.dat'));
    fileID = fopen (filename3, 'w');
        % unpack complex array again
        real_part = real(ref_rc17(i).spectralData);
        imag_part = imag(ref_rc17(i).spectralData);
        % interleave the real and imaginary part for output
        output = interleave(real_part, imag_part);
        % append max autocorrelation to the output
        output = [output ref_rc17(i).autocorrelation];
        fwrite(fileID, output, 'double', 'ieee-le');
    fclose(fileID);
end