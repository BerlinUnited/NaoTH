from numpy.fft import rfft, irfft, fft, ifft
import numpy as np


def test_01():
    x = [1, 4, 1]

    spectrum_x = fft(x)
    spectrum_y = fft(x)

    cross_correlation = ifft(spectrum_x * spectrum_y)
    print("Spectrum X: " + str(spectrum_x))
    print("Spectrum Y: " + str(spectrum_y))
    print("Correlation: " + str(cross_correlation))


def test_02():
    x = [1, 4, 1]
    spectrum_x = fft(x)
    spectrum_y = fft(x)
    spectrum_y.imag *= -1
    cross_correlation = ifft(spectrum_x * spectrum_y)

    print("Spectrum X" + str(spectrum_x))
    print("Spectrum Y" + str(spectrum_y))
    print("Correlation: " + str(cross_correlation))


if __name__ == "__main__":
    test_01()
    print("--------------------------------------")
    test_02()
    print("--------------------------------------")
    x = [1, 4, 1]
    corr = np.correlate(x, x, 'full')
    print(corr)
