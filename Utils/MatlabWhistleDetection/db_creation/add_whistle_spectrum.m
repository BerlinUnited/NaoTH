%deprecated - do not use!!!
function data = add_whistle_spectrum(data, file, numChannels, sampleRate, category)

    [~,filename,~] = fileparts(file);

    % fill the referenceWhistle fields
    whistle.name = filename;
    whistle.numChannels = numChannels;
    whistle.samplerate = sampleRate;

    % read raw data from file
    fileID = fopen (file, 'r');
        cpp_spectrum = fread(fileID, 'double', 'ieee-le');
    fclose(fileID);
    
    %% Preprocess cpp Spectrum
    % remove the correlation value from the array
    % check if new format
    if length(cpp_spectrum) == 2051
        cpp_spectrum_proc = cpp_spectrum(1:end-1);
    else
        % all read values are spectral data
        cpp_spectrum_proc = cpp_spectrum;
        % read raw data from file as float
        fileID = fopen (file, 'r');
            cpp_spectrum_float = fread(fileID, 'float', 'ieee-le');
        fclose(fileID);
        % last value is autocorrelation value
        cpp_spectrum(end + 1) = cpp_spectrum_float(end);
    end
    % convert array of doubles to array of complex values
    cpp_spectrum_proc = complex(cpp_spectrum_proc(1:2:end),cpp_spectrum_proc(2:2:end));
    
    whistle.spectralData = cpp_spectrum_proc; 
    whistle.autocorrelation = cpp_spectrum(end);
    
    %% generate raw data from conjugated complex spectrum
    fftw('planner','estimate');
    mat_raw = ifft(conj(cpp_spectrum_proc), length(cpp_spectrum_proc)*2, 'symmetric');
    whistle.rawData = mat_raw(1:1025);
    
%     figure
%     plot(whistle.raw)
%     
    %% add to list
    if isfield(data,category) 
        data.(category) = [data.(category) whistle];
    else
        data.(category) = whistle;
    end
end