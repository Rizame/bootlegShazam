#ifndef SHAZAM_WAVPROCESSING_H
#define SHAZAM_WAVPROCESSING_H

#include "../lib/dr_wav.h"
#include <string>
#include <iostream>
#include <cmath>
#include <vector>
#include "fft.h"
#include "encoding.h"
#include "matplotlibcpp.h"
#include <ctime>
#include <algorithm>
#include "sqlite3.h"

class sqlite3_db; // forward declaration

//TODO UBRAT` NAXUY SAMPLE_RATE UBIT` I SEYVIT` VNUTRI WAV
#define sample_rate 44100
#define sample_coeff 0.25f
#define window_size 1024
#define hop_size 512
#define cutoff_frequency 5000


namespace wav {
    struct Peak;

    std::vector<float> processFile(const char *fileName);

    std::vector<std::vector<float> > createWindows(const std::vector<float> &pcmFrames);

    std::vector<std::vector<float> > createSpectrogram(const std::vector<float> &pcmFrames);

    std::vector<Peak> filterPeaks(const std::vector<std::vector<float> > &spectrogram);

    void applyHammingWindow(std::vector<float> &window);

    void plotWindow(std::vector<float> &window);

    void processPeaks(std::vector<Peak> &peaks, bool toStore, std::string songName);

    std::unordered_map<uint32_t, std::vector<double> > createFingerprints(std::vector<wav::Peak> &peaks);

    float lowPassFilter(float &prev, float current);

    float const m_coeff = std::exp(-(1 / sample_rate) / (1 / M_PI * cutoff_frequency));

    struct Score {
        float offset = 0.f;
        int score = 0;
        int songId = -1;
    };

    struct Peak {
        float time;
        int bin;
        float mag;
    };

    Score scoreMatches(std::unordered_map<uint32_t, std::vector<std::pair<int, double> > > &matches,
                       std::unordered_map<uint32_t, std::vector<double> > &clips);
}


#endif //SHAZAM_WAVPROCESSING_H
