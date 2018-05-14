import numpy as np
import cpp_spectrum as cpp
import matplotlib.pyplot as plt
np.set_printoptions(threshold=np.inf)
# NOTE: works only with python 3


def bytes_from_file(filename, chunksize=2):  # chunksize is size of short
    with open(filename, "rb") as f:
        while True:
            chunk = f.read(chunksize)
            if chunk:
                yield chunk
            else:
                break


def plot_cpp_spectrum():
    t = np.arange(len(cpp.cpp_spectrum))

    # red dashes, blue squares and green triangles
    plt.plot(t, cpp.cpp_spectrum, 'r--')
    plt.show()


def compare_spectra():
    t = np.arange(len(spectrum.real[:1025]))
    # Compare spectra
    print(len(cpp.cpp_spectrum))
    print(len(spectrum.real[:1025]))

    diff = np.abs(np.abs(cpp.cpp_spectrum) - np.abs(spectrum.real[:1025]))
    print(diff > 1e-5)
    plt.plot(t, diff, 'r--')
    plt.show()


def test_autocorrelation():
    # load spectra data generated from cpp and
    return


if __name__ == "__main__":

    samples = []
    for b in bytes_from_file('data/black_loud_begin.raw'):
        i = int.from_bytes(b, byteorder='little', signed=True)
        samples.append(i)
        # print(i)

    # (1) Calculate Auto-correlation via fft and ifft
    # Flip samples
    samples = np.array(samples) / 32767  # short max
    signal2correlate = samples
    signal2correlate = np.array(list(reversed(samples)))
    # Normalize - why is this done??? - results in numbers between -1 and 1

    fft_out = np.fft.rfft(signal2correlate)
    fft_out2 = np.fft.rfft(samples)
    whistle_spectrum = fft_out
    #whistle_spectrum.imag = -fft_out.imag
    trans_spectrum = fft_out * fft_out2

    #trans_spectrum.real = fft_out.real * whistle_spectrum.real - fft_out.imag * whistle_spectrum.imag
    #trans_spectrum.imag = fft_out.real * whistle_spectrum.imag + fft_out.imag * whistle_spectrum.real

    corr = np.fft.irfft(trans_spectrum)
    #print(corr)
    t = np.arange(len(corr))
    plt.plot(t, corr, 'r--')
    plt.show()

    # TODO finish this calculation
    """
    # (2) Calculate Auto-correlation directly
    corr = np.correlate(signal2correlate, signal2correlate, 'full')
    #print(corr)

    t = np.arange(len(corr))

    # red dashes, blue squares and green triangles
    plt.plot(t, corr, 'r--')
    plt.show()
    """
