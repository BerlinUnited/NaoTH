function data = add_whistle(data, file, numChanels, sampleRate, category)

    [~,filename,~] = fileparts(file);

    % fill the referenceWhistle fields
    whistle.name = filename;
    whistle.numChannels = numChanels;
    whistle.samplerate = sampleRate;

    % read raw data from file
    fileID = fopen (file, 'r');
        raw = fread(fileID, 'int16', 'ieee-le');
    fclose(fileID);
    whistle.rawData = raw;
    
    % calculate spectrum from raw data
    [spectrum, max_auto_corr] = calc_spectrum(raw);
    
    whistle.spectralData = spectrum;
    
    whistle.autocorrelation = max_auto_corr;
    
    if isfield(data,category) 
        data.(category) = [data.(category) whistle];
    else
        data.(category) = whistle;
    end
end

