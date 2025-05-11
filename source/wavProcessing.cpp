#define DR_WAV_IMPLEMENTATION
#include "wavProcessing.h"
#define window_size 1024
#define hop_size 512

std::vector<float> processFile(const char *fileName){
    drwav wav;
    if (!drwav_init_file(&wav, fileName, NULL)) {
        throw std::logic_error("Failed to open WAV file.\n");
    }

    std::cout << "Channels: " << wav.channels << ", Sample Rate: " << wav.sampleRate << ", Total Frames: " << wav.totalPCMFrameCount << "\n";

    // total samples
    uint64_t totalSamples = wav.totalPCMFrameCount * wav.channels;
    std::vector<float> samples(totalSamples);


    uint64_t samplesRead = drwav_read_pcm_frames_f32(&wav, wav.totalPCMFrameCount, samples.data());
    drwav_uninit(&wav);

    if (samplesRead != wav.totalPCMFrameCount) {
        std::cerr << "Warning: Not all frames read.\n";
    }
    std::cout << "Total size: " << samples.size() << "\n";

    return samples;
}

std::vector<std::vector<float>> createWindows(const std::vector<float>& pcmFrames){
    std::vector<std::vector<float>> windows;

    unsigned int totalFrames = pcmFrames.size();

    for (int i = 0; i + window_size <= totalFrames; i += hop_size) {
        std::vector<float> window(pcmFrames.begin() + i, pcmFrames.begin() + i + window_size);
        applyHammingWindow(window);
        windows.push_back(window);

    }

    if(totalFrames % window_size != 0){
        int index = totalFrames - (totalFrames % window_size);
        std::vector<float> lastWindow(pcmFrames.begin() + index, pcmFrames.end());
        for(int i = lastWindow.size(); i < window_size; i++){
            lastWindow.push_back(0.0f);
        }
        windows.push_back(lastWindow);
    }
    std::cout<<"windows created succsefully, total amount: " << windows.size()<<std::endl;


    return windows;
}

void applyHammingWindow(std::vector<float>& window){
    int N = window_size;
    for (int i = 0; i < N; ++i) {
        window[i] *= 0.54 - 0.46 * cos(2 * M_PI * i / (N - 1));
    }
}


