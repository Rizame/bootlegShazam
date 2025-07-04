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


#define sample_rate 44100
#define sample_coeff 0.25f
#define window_size 1024
#define hop_size 512

namespace wav {
    struct Peak;

    std::vector<float> processFile(const char *fileName);

    std::vector<std::vector<float> > createWindows(const std::vector<float> &pcmFrames);

    std::vector<std::vector<float> > createSpectrogram(const std::vector<float> &pcmFrames);

    std::vector<Peak> filterPeaks(const std::vector<std::vector<float> > &spectrogram);

    void applyHammingWindow(std::vector<float> &window);

    void plotWindow(std::vector<float> &window);

    void plotSpectrogram(std::vector<std::vector<float> > &spectrogram);

    std::vector<std::vector<float> > applyTimestamp(std::vector<std::vector<float> > &spectrogram, float samplingRate,
                                                    float hopSize);

    void processPeaks(std::vector<Peak> &peaks, bool toStore, std::string songName);

    std::unordered_map<int, std::vector<double> > createFingerprints(std::vector<wav::Peak> &peaks);

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

    Score scoreMatches(std::unordered_map<int, std::vector<std::pair<int, double> > > &matches,
                       std::unordered_map<int, std::vector<double> > &clips);
}


#endif //SHAZAM_WAVPROCESSING_H
