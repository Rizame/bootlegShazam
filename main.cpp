#include <iostream>
#include "include/fft.h"
#include <cmath>

FFT fft;



int main() {
    FFT fft;                                         // your wrapper (N = 1024)

    /* build a 1024-sample frame – e.g. a 1 kHz sine at Fs = 44100 */
    std::vector<float> frame(1024);
    for (int n = 0; n < 1024; ++n)
        frame[n] = std::sin(2.0f * M_PI * 1000.0f * n / 44100.0f);

    const fftwf_complex* spectrum = fft.apply_fft_on_window(frame);      // run FFT

    /* print bin index, frequency, magnitude */
    for (int k = 0; k <= 1024/2; ++k) {
        float mag = std::hypot(spectrum[k][0], spectrum[k][1]); // |X[k]|
        std::cout << k << '\t'
                  << k * 44100.0f / 1024      << " Hz\t"
                  << mag                      << '\n';
    }
    return 0;
}
