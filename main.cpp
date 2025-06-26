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

    wav::createFingerPrint(peaks);

    wav::plotSpectrogram(spectrogram);
    std::cout << "\nSize of the trimmed spectrum: " << spectrogram.back().size();
    return 0;
}
