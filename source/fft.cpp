//
// Created by obbte on 05/11/25.
//

#include "../include/fft.h"
#include <cstring>
#include <iostream>

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

std::vector<float> FFT::apply_fft_on_window(const std::vector<float> &window) {
    if (window.size() != N) {
        std::cerr << "Warning: window size mismatch!" << std::endl;
    }
    std::memcpy(input_, window.data(), N * sizeof(float));
    fftwf_execute(plan_);
    std::vector<float> mag(window.size()/2+1);

    //everything crashes on i: 18984408
    // for(int i = 0; i < mag.size();i++){
    //     mag[i] = hypot(out_[i][0], out_[i][1]);
    // }
    for (size_t i = 0; i < mag.size(); i++) {
        float real = out_[i][0];
        float imag = out_[i][1];
        mag[i] = std::sqrt(real*real + imag*imag);
        //mag[i] = real * real + imag * imag;  // Power instead of magnitude
        /// MIGHT TRY SWITCHING TO power spectrum in dB
    }
    return mag; // spectrum for this frame
}
