#include <iostream>
#include "lib/wavProcessing.h"

int main() {
    std::vector samples = processFile("songs/NeverGonnaGive.wav");
    std::vector<std::vector<float>> windows = createWindows(samples);
    std::cout<<"last window: \n";
    std::vector last = windows[18000];
    for(int i = 0; i < last.size();i++){
        std::cout<<last[i]<<", ";
        if(i % 10 == 0) std::cout<<"\n";
    }

    return 0;
}
