#include "../lib/dr_wav.h"
#include <string>
#include <iostream>
#include <cmath>
#include <vector>
#include "fft.h"
#include "matplotlibcpp.h"
#include <ctime>
#include <algorithm>


#ifndef SHAZAM_WAVPROCESSING_H
#define SHAZAM_WAVPROCESSING_H

#define sample_rate 44100
#define sample_coeff 0.25f
#define window_size 1024
#define hop_size 512

namespace wav {
    std::vector<float> processFile(const char *fileName);

    std::vector<std::vector<float> > createWindows(const std::vector<float> &pcmFrames);

    std::vector<std::vector<float> > createSpectrogram(const std::vector<float> &pcmFrames);

    std::vector<float> filterPeaks(const std::vector<std::vector<float> > &spectrogram);

    void applyHammingWindow(std::vector<float> &window);

    void plotWindow(std::vector<float> &window);

    void plotSpectrogram(std::vector<std::vector<float> > &spectrogram);

    std::vector<std::vector<float> > applyTimestamp(std::vector<std::vector<float> > &spectrogram, float samplingRate,
                                                    float hopSize);
}


#endif //SHAZAM_WAVPROCESSING_H
