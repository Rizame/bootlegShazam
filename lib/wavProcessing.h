#include "../lib/dr_wav.h"
#include <string>
#include <iostream>
#include <cmath>
#include <vector>
#include "../include/fft.h"


#ifndef SHAZAM_WAVPROCESSING_H
#define SHAZAM_WAVPROCESSING_H

#define window_size 1024
#define hop_size 512

std::vector<float> processFile(const char *fileName);
std::vector<std::vector<float>> createWindows(const std::vector<float>& pcmFrames);
std::vector<std::vector<float>> createSpectrogram(const std::vector<float>& pcmFrames);
void applyHammingWindow(std::vector<float>& window);

#endif //SHAZAM_WAVPROCESSING_H
