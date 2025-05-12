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

std::vector<float> FFT::apply_fft_on_window(const std::vector<float> &window) {
    std::memcpy(input_, window.data(), N * sizeof(float));
    fftwf_execute(plan_);
    std::vector<float> mag(window.size()/2+1);

    //everything crashes on i: 18984408
    for(int i = 0; i < mag.size();i++){
        float temp = hypot(out_[i][0], out_[i][1]);
        mag[i] = temp;
    }
    return mag; // spectrum for this frame
}
