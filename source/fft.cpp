//
// Created by obbte on 05/11/25.
//

#include "../include/fft.h"
#include <cstring>

FFT::FFT() {
    input_ = fftwf_alloc_real(N);
    out_ = fftwf_alloc_complex(N / 2 + 1);
    plan_ = fftwf_plan_dft_r2c_1d(N, input_, out_, FFTW_MEASURE);
}


FFT::~FFT() {
    fftwf_destroy_plan(plan_);
    fftwf_free(input_);
    fftwf_free(out_);
}

fftwf_complex *FFT::apply_fft_on_window(const std::vector<float> &window) {
    std::memcpy(input_, window.data(), N * sizeof(float));

    fftwf_execute(plan_);
    return out_; // spectrum for this frame
}
