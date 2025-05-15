#include <iostream>
#include "include/wavProcessing.h"
#include "matplotlibcpp.h"

int main() {
    std::vector samples = wav::processFile("songs/NeverGonnaGive.wav");
    std::vector<std::vector<float> > spectrogram = wav::createSpectrogram(samples);
    std::cout << "spectrum: \n";
    std::vector last = spectrogram[18000];
    // for (int i = 0; i < last.size(); i++) {
    //     std::cout << last[i] << ", ";
    //     if (i % 10 == 0) std::cout << "\n";
    // }
    auto timeMatrix = wav::applyTimestamp(spectrogram, 44100.0f, 512.0f);
    std::cout << timeMatrix.back().back() << "s" << std::endl;

    wav::plotSpectrogram(spectrogram);

    return 0;
}
