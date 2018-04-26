function result = maxcrosscorr(raw_samples, reference_spectrum, reference_max)

    %% Calculate correlation function in matlab
    
    % normalize the samples like the cpp implementation
    short_max = 32767;
    raw_samples_norm = raw_samples / short_max;

    n = 2^nextpow2(size(raw_samples_norm,1));
    
    mat_spectrum = fft(raw_samples_norm, n * 3);

    % // real x real - imag x imag
    % fftIn[j][0] = realFFTIn * realFFTCmp - imagFFTIn * imagFFTCmp;
    % // real x imag + imag x real
    % fftIn[j][1] = realFFTIn * imagFFTCmp + imagFFTIn * realFFTCmp;
    intermediate_result = mat_spectrum(1:1025) .* reference_spectrum;
    mat_correlation_func = ifft(intermediate_result, length(raw_samples_norm)*2) * 2048;
    result = max(abs(mat_correlation_func)) / reference_max;

end

% bool WhistleDetector::detectWhistles(int channel)
% {
%   bool whistleDetected = false;
%   fftw_execute(planFFT); // compute the FFT - repeat as needed
% 
%   for(size_t idxWhistle = 0; idxWhistle < referenceWhistleSpectra.size(); idxWhistle++)
%   {
%     
%     // Now multiply the FFT of dataIn with the conjugate FFT of cmpData
%     for(int j = 0; j < WHISTLE_BUFF_LEN + 1; j++)
%     {
%       const double realFFTIn = fftOut[j][0];
%       const double imagFFTIn = fftOut[j][1];
% 
%       const double realFFTCmp = referenceWhistleSpectra[idxWhistle][j][0];
%       const double imagFFTCmp = referenceWhistleSpectra[idxWhistle][j][1];
% 
%       // real x real - imag x imag
%       fftIn[j][0] = realFFTIn * realFFTCmp - imagFFTIn * imagFFTCmp;
%       // real x imag + imag x real
%       fftIn[j][1] = realFFTIn * imagFFTCmp + imagFFTIn * realFFTCmp;
%     }
%     fftw_execute(planIFFT); // calculate the IFFT which is now the same as the correlation
% 
%     // Post Processing:
%     // Find the maximum of the correlation and weight by the
%     // highest value possible, which is given by the auto correlation
% 
%     double correlation = 0.0;
%     for(int j = 0; j < WHISTLE_FFT_LEN; j++)
%     {
%       if(correlation < fabs(correlatedBuffer[j]))
%       {
%         correlation = fabs(correlatedBuffer[j]);
%       }
%     }
%     if(correlation / referenceWhistleAutoCorrelationMaxima[idxWhistle] >= threshold)
%     {
%       std::cout << "Whistle " << referenceWhistleNames[idxWhistle] << "(No. " << idxWhistle << ") maxCorr " << correlation << " ref corr " << referenceWhistleAutoCorrelationMaxima[idxWhistle] << std::endl;
%       std::cout << "Whistle " << referenceWhistleNames[idxWhistle] << "(No. " << idxWhistle << ") detected! " << correlation / referenceWhistleAutoCorrelationMaxima[idxWhistle] << std::endl;
%       whistleDetected = true;
%       WhistlePercept::Whistle recWhistle();
%       recognizedWhistles.push_back(recWhistle);
%       if(!checkAllWhistles)
%       {
%         return true;
%       }
%     }
%   }
%   if(whistleDetected)
%   {
%     std::cout << "@ Audio Channel " << channel;
%     /*
%     if(testFileMode)
%     {
%       std::cout << " on Position " << (audioTestFile.tellg() / (sizeof(short) * NUM_CHANNELS_RX)) << std::endl;
%     }
%     */
%   }
%   return whistleDetected;
% }