#define DR_WAV_IMPLEMENTATION
#include "../include/wavProcessing.h"


std::vector<float> wav::processFile(const char *fileName) {
    drwav wav;
    if (!drwav_init_file(&wav, fileName, NULL)) {
        throw std::logic_error("Failed to open WAV file.\n");
    }

    std::cout << "Channels: " << wav.channels << ", Sample Rate: " << wav.sampleRate << ", Total Frames: " << wav.
            totalPCMFrameCount << "\n";

    // total samples
    uint64_t totalSamples = wav.totalPCMFrameCount * wav.channels;
    std::vector<float> samples(totalSamples);


    uint64_t samplesRead = drwav_read_pcm_frames_f32(&wav, wav.totalPCMFrameCount, samples.data());
    drwav_uninit(&wav);

    if (samplesRead != wav.totalPCMFrameCount) {
        std::cerr << "Warning: Not all frames read.\n";
    }
    std::cout << "Total number of samples: " << samples.size() << "\n";

    return samples;
}

std::vector<std::vector<float> > wav::createWindows(const std::vector<float> &pcmFrames) {
    std::vector<std::vector<float> > windows;
    FFT fftTool;

    unsigned int totalFrames = pcmFrames.size();

    for (int i = 0; i + window_size <= totalFrames; i += hop_size) {
        std::vector<float> window(pcmFrames.begin() + i, pcmFrames.begin() + i + window_size);
        applyHammingWindow(window);
        windows.push_back(window);
    }

    if ((totalFrames - window_size) % hop_size != 0) {
        int index = totalFrames - (totalFrames % window_size);
        std::vector<float> lastWindow(pcmFrames.begin() + index, pcmFrames.end());
        for (int i = lastWindow.size(); i < window_size; i++) {
            lastWindow.push_back(0.0f);
        }
        windows.push_back(lastWindow);
    }
    std::cout << "Windows created succsefully, total amount: " << windows.size() << std::endl;


    return windows;
}

void wav::plotWindow(std::vector<float> &window) {
    matplotlibcpp::plot(window);
    matplotlibcpp::show();
}

void wav::plotSpectrogram(std::vector<std::vector<float> > &spectrogram) {
    // matplotlibcpp::;
    // matplotlibcpp::show();
}


std::vector<std::vector<float> > wav::createSpectrogram(const std::vector<float> &pcmFrames) {
    FFT customFft;
    std::vector<std::vector<float> > windows = createWindows(pcmFrames);
    std::vector<std::vector<float> > spectrogram(windows.size());
    for (int i = 0; i < windows.size(); i++) {
        spectrogram[i] = (customFft.apply_fft_on_window(windows[i]));
    }
    return spectrogram;
}

void wav::applyHammingWindow(std::vector<float> &window) {
    int N = window_size;
    for (int i = 0; i < N; ++i) {
        window[i] *= 0.54 - 0.46 * cos(2 * M_PI * i / (N - 1));
    }
}



