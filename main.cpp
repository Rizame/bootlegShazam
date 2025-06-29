#include <iostream>
#include "include/wavProcessing.h"
#include "matplotlibcpp.h"
#include "include/encoding.h"

int main() {
    std::vector samples = wav::processFile("songs/NeverGonnaGive.wav");
    std::vector<std::vector<float> > spectrogram = wav::createSpectrogram(samples);

    auto timeMatrix = wav::applyTimestamp(spectrogram, 44100.0f * sample_coeff, 512.0f);
    std::cout << timeMatrix.back().back() << "s" << std::endl;

    auto peaks = wav::filterPeaks(spectrogram);

    //wav::processPeaks(peaks, true);

    //wav::plotSpectrogram(spectrogram);

    std::vector samplesInput = wav::processFile("songs/TrimmedNever.wav");
    std::vector<std::vector<float> > spectrogramInput = wav::createSpectrogram(samplesInput);
    auto peaksInput = wav::filterPeaks(spectrogramInput);
    wav::processPeaks(peaksInput, false);

    return 0;
}
