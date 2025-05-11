#include "../lib/dr_wav.h"
#include <string>
#include <iostream>
#include <cmath>
#include <vector>


#ifndef SHAZAM_WAVPROCESSING_H
#define SHAZAM_WAVPROCESSING_H

std::vector<float> processFile(const char *fileName);
std::vector<std::vector<float>> createWindows(const std::vector<float>& pcmFrames);
void applyHammingWindow(std::vector<float>& window);

#endif //SHAZAM_WAVPROCESSING_H
