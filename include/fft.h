//
// Created by obbte on 05/11/25.
//

#ifndef FFT_H
#define FFT_H
#include <vector>
#include <fftw3.h>
#include <math.h>


class FFT {
public:
    FFT();

    ~FFT();

    std::vector<float> apply_fft_on_window(const std::vector<float> &window); // copies, executes, returns out_
private:
    fftwf_plan plan_;
    float *input_;
    fftwf_complex *out_;
    static constexpr int N = 1024;
    static constexpr int H = 512; // hop
};
#endif //FFT_H
