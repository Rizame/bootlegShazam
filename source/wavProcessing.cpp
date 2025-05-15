#define DR_WAV_IMPLEMENTATION
#include "../include/wavProcessing.h"


std::vector<float> wav::processFile(const char *fileName) {
    drwav wav;
    if (!drwav_init_file(&wav, fileName, NULL)) {
        throw std::logic_error("Failed to open WAV file.\n");
    }

    std::cout << "Channels: " << wav.channels << ", Sample Rate: " << wav.sampleRate << ", Total Frames: " << wav.
            totalPCMFrameCount << "\n";

    uint64_t totalSamples = wav.totalPCMFrameCount * wav.channels;
    std::vector<float> samples(totalSamples);

    uint64_t samplesRead = drwav_read_pcm_frames_f32(&wav, wav.totalPCMFrameCount, samples.data());
    drwav_uninit(&wav);

    if (samplesRead != wav.totalPCMFrameCount) {
        std::cerr << "Warning: Not all frames read.\n";
    }

    std::cout << "Total number of samples before:: " << samples.size() << "\n";

    std::vector<float> reducedSamples;
    reducedSamples.reserve((samples.size() * sample_coeff));

    for (size_t i = 0; i < samples.size(); ++i) {
        if (i % 4 == 0) {
            reducedSamples.push_back(samples[i]);
        }
    }

    std::cout << "Total number of samples after:" << reducedSamples.size() << "\n";

    return reducedSamples;
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

void wav::plotSpectrogram(std::vector<std::vector<float> > &spec) {
    const int rows = static_cast<int>(spec.size());
    const int cols = static_cast<int>(spec[0].size());

    /* 1 · flatten into a single contiguous std::vector<float>  */
    std::vector<float> img(rows * cols);
    for (int r = 0; r < rows; ++r)
        std::copy(spec[r].begin(), spec[r].end(), img.begin() + r * cols);

    matplotlibcpp::imshow(img.data(), rows, cols, /*colors=*/1,
                          {
                              {"origin", "lower"},
                              {"interpolation", "nearest"},
                              {"aspect", "auto"},
                              {"cmap", "magma"}
                          });

    matplotlibcpp::xlabel("Time frame"); // use timeMatrix to build real-world ticks
    matplotlibcpp::ylabel("Frequency bin"); // if you like — omitted here
    matplotlibcpp::show();
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


std::vector<float> wav::filterPeaks(const std::vector<std::vector<float> > &spectrogram) {
    std::vector<float> peaks;
    int ranges[6] = {10, 20, 40, 80, 160, 512};

    for (const auto &spectrum: spectrogram) {
        float curRangePeak = 0.0f;
        int curRange = 0;
        int j = 0;
        while (j < ranges[5]) {
            if (j > ranges[curRange]) {
                peaks.push_back(curRangePeak);
                curRangePeak = 0.0;
                curRange++;
            }
            if (spectrum[j] > curRangePeak) {
                curRangePeak = spectrum[j];
            }
            j++;
        }
        peaks.push_back(curRangePeak);
    }

    float mean = std::accumulate(peaks.begin(), peaks.end(), 0.0f) / static_cast<float>(peaks.size());
    std::cout << "Mean: " << mean << "\n";
    std::cout << "Peaks before: " << peaks.size() << "\n";

    std::erase_if(peaks,
                  [mean](float v) { return v <= mean; });

    std::cout << "Peaks after: " << peaks.size() << "\n";
    for (float peak: peaks) {
        std::cout << peak << "\n";
    }

    return peaks;
};


void wav::applyHammingWindow(std::vector<float> &window) {
    for (int i = 0; i < window_size; ++i) {
        window[i] *= 0.54 - 0.46 * cos(2 * M_PI * i / (window_size - 1));
    }
}

std::vector<std::vector<float> > wav::applyTimestamp(std::vector<std::vector<float> > &spectrogram, float samplingRate,
                                                     float hopSize) {
    // every item in spectrogram was created from windows that were sliced
    // with a rate of 44 100Hz, we got 1024 samples from every window
    // hence to find the timestamp for a corresponding spectrum we use the
    // sample, sample rate and hop size
    std::vector timeMatrix(spectrogram);

    double hopTime = hopSize / samplingRate;
    for (int i = 0; i < spectrogram.size(); i++) {
        std::vector<time_t> window(spectrogram[i].size());
        auto windowTime = i * hopTime;
        for (int j = 0; j < spectrogram[i].size(); j++) {
            double delta_time = j / samplingRate;
            timeMatrix[i][j] = windowTime + delta_time;
        }
    }
    return timeMatrix;
}
