#define DR_WAV_IMPLEMENTATION

#include "../include/wavProcessing.h"
#include "../include/db.h"


std::vector<float> wav::processFile(const char *fileName) {
    drwav wav;
    if (!drwav_init_file(&wav, fileName, NULL)) {
        throw std::logic_error("Failed to open WAV file.\n");
    }

    std::cout << "Channels: " << wav.channels << ", Sample Rate: " << wav.sampleRate << ", Total Frames: " << wav.
            totalPCMFrameCount << std::endl;

    uint64_t totalSamples = wav.totalPCMFrameCount * wav.channels;
    std::vector<float> samples(totalSamples);

    uint64_t samplesRead = drwav_read_pcm_frames_f32(&wav, wav.totalPCMFrameCount, samples.data());
    drwav_uninit(&wav);

    if (samplesRead != wav.totalPCMFrameCount) {
        std::cerr << "Warning: Not all frames read.\n";
    }

    std::cout << "Total number of samples before:: " << samples.size() << std::endl;

    std::vector<float> reducedSamples;
    reducedSamples.reserve((samples.size() * sample_coeff));

    for (size_t i = 0; i < samples.size(); ++i) {
        if (i % 4 == 0) {
            reducedSamples.push_back(samples[i]);
        }
    }

    std::cout << "Total number of samples after:" << reducedSamples.size() << std::endl;

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
    const int cols = static_cast<int>(spec.size());
    const int rows = static_cast<int>(spec[0].size());

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

    matplotlibcpp::xlabel("Frequency"); // use timeMatrix to build real-world ticks
    matplotlibcpp::ylabel("Amplitude"); // if you like — omitted here
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


std::vector<wav::Peak> wav::filterPeaks(const std::vector<std::vector<float> > &spectrogram) {
    std::vector<Peak> peaks;
    double hopTime = hop_size / (sample_rate * sample_coeff);
    int ranges[6] = {10, 20, 40, 80, 160, 512};
    // TODO check if we need to know which peak belongs to specific window

    for (int i = 0; i < spectrogram.size(); i++) {
        Peak curRangePeak = {0.0f, 0, 0.0f};
        int curRange = 0;
        int j = 0;
        while (j < ranges[5]) {
            if (j > ranges[curRange]) {
                peaks.push_back(curRangePeak);
                curRangePeak = {0, 0, 0.0};
                curRange++;
            }
            if (spectrogram[i][j] > curRangePeak.mag) {
                curRangePeak = {static_cast<float>((i + 1) * hopTime), j, spectrogram[i][j]};
            }
            j++;
        }
        peaks.push_back(curRangePeak);
    }

    float sum = std::accumulate(peaks.begin(), peaks.end(), 0.0f,
                                [](float acc, const Peak p) { return acc + p.mag; });
    float mean = sum / static_cast<float>(peaks.size());
    std::cout << "Mean: " << mean << std::endl;
    std::cout << "Peaks before: " << peaks.size() << std::endl;

    std::erase_if(peaks,
                  [mean](Peak v) { return v.mag <= mean; });

    std::cout << "Peaks after: " << peaks.size() << std::endl;

    return peaks;
};


void wav::applyHammingWindow(std::vector<float> &window) {
    for (int i = 0; i < window_size; ++i) {
        window[i] *= 0.54 - 0.46 * cos(2 * M_PI * i / (window_size - 1));
    }
}

/* Every item in spectrogram was created from windows that were sliced
 with a rate of 44 100Hz, we got 1024 samples from every window
 hence to find the timestamp for a corresponding spectrum we use the
 sample, sample rate and hop size */
std::vector<std::vector<float> > wav::applyTimestamp(std::vector<std::vector<float> > &spectrogram, float samplingRate,
                                                     float hopSize) {
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

std::unordered_map<int, std::vector<double> > wav::createFingerprints(std::vector<wav::Peak> &peaks) {
    int TARGET_ZONE_SIZE = 4;
    std::unordered_map<int, std::vector<double> > fingerprints;
    // TODO check proyob with int and double

    wav::Peak anchor{0, 0, 0};
    for (int i = 0; i < peaks.size() - TARGET_ZONE_SIZE; i++) {
        anchor = peaks[i];
        for (int j = 1; j <= TARGET_ZONE_SIZE; j++) {
            float d_time = std::abs(anchor.time - peaks[i + j].time);
            uint32_t hash = encoding::encode(anchor.bin, peaks[i + j].bin, d_time);
            fingerprints[hash].emplace_back(anchor.time);
        }
    }

    return fingerprints;
}

/*
@param matches is of type [song_id] = {hash_x, a_time_x, ...}
@param clips is of type [hash] = {a_time1, a_time2, ...}

@return song_id of the matches song
 */
std::pair<int, int> wav::scoreMatches(std::unordered_map<int, std::vector<std::pair<int, double> > > &matches,
                                      std::unordered_map<int, std::vector<double> > &clips) {
    std::pair<int, int> topScore; // bin of the offset and song id
    std::unordered_map<int, int> histogram;


    for (const auto &[song_id, hashes]: matches) {
        for (int i = 0; i < hashes.size(); i++) {
            auto match_hash_time = hashes[i];
            auto clip_anchor_times = clips[match_hash_time.first];

            for (int j = 0; j < clip_anchor_times.size(); j++) {
                auto offset = clip_anchor_times[j] - match_hash_time.second;
                int bin = std::round(offset / 0.05);
                histogram[bin]++;
                if (histogram[bin] > histogram[topScore.first]) {
                    topScore.first = bin;
                    topScore.second = song_id;
                }
            }
        }
    }

    return topScore;
}

/*Function that calls hashing on every anchor point and  */
void wav::processPeaks(std::vector<Peak> &peaks, bool toStore) {
    sqlite3_db db("store.db");
    auto fingerPrints = createFingerprints(peaks);

    if (toStore) {
        //db.drop_db(2);
        //db.db_create();
        // auto song_id = db.db_insert_song("Never gonna give you up");
        auto song_id = db.db_insert_song("TogetherForever");

        db.db_process_fingerPrints(fingerPrints, song_id);
    } else {
        std::unordered_map<int, std::vector<std::pair<int, double> > > matches = db.db_match_fingerPrints(fingerPrints);
        auto result = scoreMatches(matches, fingerPrints);

        std::cout << "Offset: " << result.first * 50 / 1000.0 << "s" <<
                ", Song Id: " << result.second << std::endl;
    }
}
