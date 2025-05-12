#include <iostream>
#include "include/wavProcessing.h"

int main() {
    std::vector samples = wav::processFile("songs/NeverGonnaGive.wav");
    std::vector<std::vector<float>> spectrogram = wav::createSpectrogram(samples);
    std::cout<<"spectrum: \n";
    std::vector last = spectrogram[18000];
    for(int i = 0; i < last.size();i++){
        std::cout<<last[i]<<", ";
        if(i % 10 == 0) std::cout<<"\n";
    }

    return 0;
}
