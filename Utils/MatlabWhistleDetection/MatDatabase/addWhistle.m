function data = addWhistle(data, file, numChanels, sampleRate, category)
%ADDWHISTLE Summary of this function goes here
%   Detailed explanation goes here
    [path_to_file,filename,~] = fileparts(file);

    % fill the referenceWhistle fields
    whistle.name = filename;
    whistle.numChannels = numChanels;
    whistle.samplerate = sampleRate;

    % read raw data from file
    fileID = fopen (file, 'r');
        raw = fread(fileID, 'int16', 'ieee-le');
    fclose(fileID);
    whistle.rawData = raw;
    % TODO how to handle data from multiple channels, 
    % should each channel be saved individually?
    
    % calculate spectrum from raw data
    [spectrum, max_auto_corr] = calculate_spectrum(raw);
    
    % read spectrum from dat file
%     spectrum_filename = strcat(filename,'.dat');
%     disp(fullfile(path_to_file,spectrum_filename))
%     fileID = fopen (fullfile(path_to_file,spectrum_filename), 'r');
%         spectrum = fread(fileID, 'double', 'ieee-le');
%     fclose(fileID);
%     calc_corr = spectrum(end);
%     spectrum = spectrum(1:end-1); % remove correlation value from spectrum
%     spectrum = complex(spectrum(1:2:end),spectrum(2:2:end));
    whistle.spectralData = spectrum;
    
    whistle.autocorrelation = max_auto_corr;
    
    if isfield(data,category) 
        data.(category) = [data.(category) whistle];
    else
        data.(category) = whistle;
    end
end

