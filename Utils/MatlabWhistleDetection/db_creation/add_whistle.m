function data = add_whistle(data, file, numChanels, sampleRate, category)

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
    
    whistle.spectralData = spectrum;
    
    whistle.autocorrelation = max_auto_corr;
    
    if isfield(data,category) 
        data.(category) = [data.(category) whistle];
    else
        data.(category) = whistle;
    end
end

