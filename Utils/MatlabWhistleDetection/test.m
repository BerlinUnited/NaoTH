clear variables;
reference = audioread('WhistleTest01.wav');

%add zeros in the end so the total length is 2*length(reference)
num_zeros = length(reference);
reference_zeros = [reference; zeros(num_zeros, 1)];

% calculate the spectrum
reference_spectrum = fft(reference_zeros);
reference_spectrum_conj = conj(reference_spectrum);

intermediate = reference_spectrum .* reference_spectrum_conj;

correlation = ifft(intermediate);
plot(correlation);


