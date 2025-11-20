#define DR_WAV_IMPLEMENTATION

#include "../include/wavProcessing.h"
#include "../include/db.h"
#include "matplotlibcpp.h"
#include <fstream>

namespace plt = matplotlibcpp;

float wav::lowPassFilter(float &prev, float current) {
    prev = m_coeff * prev + (1.f - m_coeff) * current;
    return prev;
}
const std::vector<float> firCoeffs = {
    5.090512591120273551e-04f, 3.701705076207541484e-04f, -1.895406979139654666e-06f, 
    -4.233430830147712882e-04f, -6.532721061770292888e-04f, -5.108616168124644266e-04f, 
    2.545338277795945264e-06f, 6.495850667553869666e-04f, 1.037034247007460755e-03f, 
    8.294661719477567632e-04f, -3.804363239655411758e-06f, -1.073399626230180396e-03f, 
    -1.713652239880336268e-03f, -1.365121785790865275e-03f, 5.593373138170417376e-06f, 
    1.737366866114065187e-03f, 2.746637304013767409e-03f, 2.165488683698141639e-03f, 
    -7.799958201055946696e-06f, -2.695898014288950189e-03f, -4.219332848445962909e-03f, 
    -3.294856866149167233e-03f, 1.028547073530048353e-05f, 4.026786250766050367e-03f, 
    6.254462291693769528e-03f, 4.850742339792736384e-03f, -1.289373688738350981e-05f, 
    -5.855462204321574063e-03f, -9.055886075348558092e-03f, -7.000045299957196980e-03f, 
    1.546086964468046737e-05f, 8.410377995480847191e-03f, 1.300665802291149989e-02f, 
    1.006663986302940711e-02f, -1.782556648898551144e-05f, -1.216720920259029871e-02f, 
    -1.893534118322297288e-02f, -1.478068130714906350e-02f, 1.983924465911939103e-05f, 
    1.830737066394730675e-02f, 2.902080141542721925e-02f, 2.319204301191224449e-02f, 
    -2.137537718404577088e-05f, -3.074721147781563080e-02f, -5.131359891706169241e-02f, 
    -4.396581059889224929e-02f, 2.233744306657556476e-05f, 7.439568780633784573e-02f, 
    1.585025067807742660e-01f, 2.247522042584441937e-01f, 2.498588726316418873e-01f, 
    2.247522042584441937e-01f, 1.585025067807742383e-01f, 7.439568780633784573e-02f, 
    2.233744306657556137e-05f, -4.396581059889223542e-02f, -5.131359891706168547e-02f, 
    -3.074721147781562386e-02f, -2.137537718404576749e-05f, 2.319204301191224449e-02f, 
    2.902080141542721578e-02f, 1.830737066394729981e-02f, 1.983924465911938425e-05f, 
    -1.478068130714906350e-02f, -1.893534118322297288e-02f, -1.216720920259029871e-02f, 
    -1.782556648898550467e-05f, 1.006663986302940537e-02f, 1.300665802291149295e-02f, 
    8.410377995480843721e-03f, 1.546086964468046398e-05f, -7.000045299957194378e-03f, 
    -9.055886075348556358e-03f, -5.855462204321569726e-03f, -1.289373688738350473e-05f, 
    4.850742339792735516e-03f, 6.254462291693766926e-03f, 4.026786250766050367e-03f, 
    1.028547073530047506e-05f, -3.294856866149165498e-03f, -4.219332848445962909e-03f, 
    -2.695898014288948454e-03f, -7.799958201055946696e-06f, 2.165488683698139471e-03f, 
    2.746637304013766108e-03f, 1.737366866114063019e-03f, 5.593373138170410600e-06f, 
    -1.365121785790865275e-03f, -1.713652239880333883e-03f, -1.073399626230179746e-03f, 
    -3.804363239655407100e-06f, 8.294661719477561127e-04f, 1.037034247007460755e-03f, 
    6.495850667553857739e-04f, 2.545338277795943993e-06f, -5.108616168124638845e-04f, 
    -6.532721061770288551e-04f, -4.233430830147712882e-04f, -1.895406979139653396e-06f, 
    3.701705076207541484e-04f, 5.090512591120273551e-04f
};

std::vector<float> wav::resampleTo44100(const std::vector<float>& input, int inputSampleRate) {
    double ratio = 44100.0 / inputSampleRate;
    
    size_t outputSize = (size_t)(input.size() * ratio);
    std::vector<float> output(outputSize);
    
    for (size_t i = 0; i < outputSize; ++i) {
        double srcPos = i / ratio;
        
        // Split into integer and fractional parts
        size_t idx = (size_t)srcPos;              
        double frac = srcPos - idx;
        
        if (idx + 1 < input.size()) {
            // Linear interpolation between two adjacent samples
            output[i] = input[idx] * (1.0 - frac) + input[idx + 1] * frac;
        } else {
            // At the very end
            output[i] = input[idx];
        }
    }
    
    return output;
}

void wav::recreateSound(std::vector<float> &input) {
    const char* outputFile = "testOutput.wav";
    drwav_data_format format;
    format.container = drwav_container_riff; // Standard WAV
    format.format   = DR_WAVE_FORMAT_IEEE_FLOAT; // Float PCM
    format.channels = 1;
    format.sampleRate = 11025;
    format.bitsPerSample = 32; // float

    drwav outWav;
    if (!drwav_init_file_write(&outWav, outputFile, &format, nullptr)) {
        std::cerr << "Failed to create output WAV.\n";
        return;
    }

    drwav_write_pcm_frames(&outWav, input.size(), input.data());
    drwav_uninit(&outWav);
}

std::vector<float> wav::applyFIR(std::vector<float> &input) {
    const int filterOrder = 101;
    std::vector<float> buffer(filterOrder, 0);
    std::vector<float> output(input.size(), 0);
    int bufferIndex = 0;
    
    for (size_t i = 0; i < input.size(); i++) {
        // Insert new sample in circular buffer
        buffer[bufferIndex] = input[i];

        float sum = 0.0f;
        int idx = bufferIndex;
        for (int j = 0; j < filterOrder; j++) {
            sum += buffer[idx] * firCoeffs[j];
            idx = (idx == 0) ? (filterOrder-1) : (idx - 1);
        }
        output[i] = sum;
        
        bufferIndex = (bufferIndex + 1) % filterOrder;
    }
    int delay = (filterOrder - 1)/2;
    output.erase(output.begin(), output.begin() + delay);
    /// compensating for the delay that was caused by FIR

    return output;
}

std::vector<float> wav::processFile(const char *fileName) {
    drwav wav;
    if (!drwav_init_file(&wav, fileName, NULL)) {
        throw std::logic_error("Failed to open WAV file.\n");
    }
    
    std::cout << "Channels: " << wav.channels 
              << ", Sample Rate: " << wav.sampleRate 
              << ", Total Frames: " << wav.totalPCMFrameCount << std::endl;

    if (wav.sampleRate < 16000) {
        drwav_uninit(&wav);
        throw std::logic_error("Minimum 16000 Hz rate required.");
    }

    
    uint64_t totalSamples = wav.totalPCMFrameCount * wav.channels;
    std::vector<float> samples(totalSamples);

    uint64_t framesRead = drwav_read_pcm_frames_f32(&wav, wav.totalPCMFrameCount, samples.data());
    drwav_uninit(&wav);
    
    if (framesRead != wav.totalPCMFrameCount) {
        std::cerr << "Warning: Not all frames read.\n";
    }
    
    // stereo to mono
    std::vector<float> monoSamples;
    if (wav.channels == 1) {
        monoSamples = samples;
    } else if (wav.channels == 2) {
        monoSamples.reserve(framesRead);
        for (size_t i = 0; i < framesRead; ++i) {
            float left = samples[i * 2 + 0];
            float right = samples[i * 2 + 1];
            monoSamples.push_back((left + right) * 0.5f);
        }
    }

    if(wav.sampleRate != sample_rate){
        monoSamples = resampleTo44100(monoSamples, wav.sampleRate);
    }
    // FIR lowpass filter
    std::vector<float> filtered = applyFIR(monoSamples);
    // std::vector<float> filtered(monoSamples);

    // downscale
    const int decimationFactor = 4;
    std::vector<float> downsampled;
    downsampled.reserve(filtered.size() / decimationFactor);

    for (size_t i = 0; i < filtered.size(); i += decimationFactor) {
        downsampled.push_back(filtered[i]);
    }


    std::cout << "Original samples: " << monoSamples.size() << std::endl;
    std::cout << "Filtered samples: " << filtered.size() << std::endl;
    std::cout << "Downsampled: " << downsampled.size() << std::endl;

    return downsampled;
}


std::vector<std::vector<float> > wav::createWindows(const std::vector<float> &pcmFrames) {
    std::vector<std::vector<float> > windows;

    unsigned int totalFrames = pcmFrames.size();

    for (int i = 0; i + window_size <= totalFrames; i += hop_size) {
        std::vector<float> window(pcmFrames.begin() + i, pcmFrames.begin() + i + window_size);
        applyHammingWindow(window);
        windows.push_back(window);
    }

    int lastWindowStart = windows.size() * hop_size;
    if (lastWindowStart < totalFrames) {
        std::vector<float> lastWindow(pcmFrames.begin() + lastWindowStart, pcmFrames.end());
        
        lastWindow.resize(window_size, 0.0f);
        
        applyHammingWindow(lastWindow);
        windows.push_back(lastWindow);
    }
    
    std::cout << "Windows created successfully, total amount: " << windows.size() << std::endl;
    return windows;
}

void wav::plotWindow(std::vector<float> &window) {
    matplotlibcpp::plot(window);
    matplotlibcpp::show();
}

void plotSpectrogram(const std::vector<std::vector<float>>& spectrogram) {
    int num_frames = spectrogram.size();
    int num_bins = spectrogram[0].size();

    // Create 2D array for heatmap (convert to vector of vectors for matplotlib)
    std::vector<std::vector<double>> spec_data(num_frames, std::vector<double>(num_bins));

    double max_power = 0.0;
    double min_power = std::numeric_limits<double>::max();

    // Convert to double and normalize for better visualization
    for (int i = 0; i < num_frames; i++) {
        for (int j = 0; j < num_bins; j++) {
            double val = static_cast<double>(spectrogram[i][j]);

            // Convert to dB scale for better visualization
            val = 10.0 * std::log10(val + 1e-10);  // Add small value to avoid log(0)

            spec_data[i][j] = val;
            max_power = std::max(max_power, val);
            min_power = std::min(min_power, val);
        }
    }

    std::cout << "Spectrogram power range: " << min_power << " to " << max_power << " dB" << std::endl;

    // Create matplotlib figure
    plt::figure_size(1400, 600);

    // Create heatmap
    // Flatten 2D array to 1D for imshow (needs row-major order)
    std::vector<float> flat_data;
    flat_data.reserve(num_frames * num_bins);

    // Transpose while flattening: store by frequency bins (columns first)
    for (int j = 0; j < num_bins; j++) {
        for (int i = 0; i < num_frames; i++) {
            flat_data.push_back(spec_data[i][j]);
        }
    }

    // Use imshow for heatmap
    // Parameters: data pointer, rows (num_bins), columns (num_frames), colors (1 for grayscale)
    plt::imshow(flat_data.data(), num_bins, num_frames, 1,
                {{"cmap", "viridis"}, {"aspect", "auto"}});

    // Labels and title
    plt::title("Spectrogram (dB Scale)");
    plt::xlabel("Time Frames");
    plt::ylabel("Frequency Bins");

    // Set axis labels with actual values
    int frame_interval = std::max(1, num_frames / 10);  // Show ~10 time labels
    int bin_interval = std::max(1, num_bins / 10);      // Show ~10 frequency labels

    std::vector<int> frame_ticks;
    std::vector<std::string> frame_labels;
    for (int i = 0; i < num_frames; i += frame_interval) {
        frame_ticks.push_back(i);
        double time_sec = (i * hop_size) / (sample_rate * sample_coeff);
        frame_labels.push_back(std::to_string(static_cast<int>(time_sec)) + "s");
    }

    std::vector<int> bin_ticks;
    std::vector<std::string> bin_labels;
    for (int i = 0; i < num_bins; i += bin_interval) {
        bin_ticks.push_back(i);
        double freq_hz = i * 10.7666;  // Your frequency resolution
        bin_labels.push_back(std::to_string(static_cast<int>(freq_hz)) + "Hz");
    }

    plt::xticks(frame_ticks, frame_labels);
    plt::yticks(bin_ticks, bin_labels);

    plt::tight_layout();
    plt::show();
}

std::vector<std::vector<float> > wav::createSpectrogram(const std::vector<float> &inputSamples) {
    FFT customFft;
    std::vector<std::vector<float> > windows = createWindows(inputSamples);
    std::vector<std::vector<float> > spectrogram(windows.size());

    for (int i = 0; i < windows.size(); i++) {
        spectrogram[i] = (customFft.apply_fft_on_window(windows[i]));
    }

    std::cout << "Spectrogram created: " << spectrogram.size() << " frames, " 
              << spectrogram[0].size() << " frequency bins" << std::endl;
    
    if (std::isnan(spectrogram[0][0]) || std::isinf(spectrogram[0][0])) {
        std::cerr << "ERROR: Spectrogram contains invalid values!" << std::endl;
    }

    //plotSpectrogram(spectrogram);
    return spectrogram;
}



std::vector<wav::Peak> wav::filterPeaks(const std::vector<std::vector<float> > &spectrogram) {
    double hopTime = hop_size / (sample_rate * sample_coeff);
    int ranges[7] = {0, 10, 20, 40, 80, 160, 512};
    int numBands = 6;

    std::vector<Peak> peaks;
    std::vector<std::vector<Peak>> peaksNew(numBands);

    for (int i = 0; i < spectrogram.size(); i++) {
        double curTime = i*hopTime;
        for(int band = 0; band < numBands;band++){
            int startBin = ranges[band] + (band == 0 ? 1 : 0);
            int endBin = ranges[band + 1];

            Peak bandPeak = {static_cast<float>(curTime), 0, 0.0f};

            for(int bin = startBin; bin < endBin;bin++){
                if(spectrogram[i][bin] > bandPeak.mag){
                    bandPeak.bin = bin;
                    bandPeak.mag = spectrogram[i][bin];
                }
            }

            if (bandPeak.mag > 0.f) {
                peaksNew[band].push_back(bandPeak);
            }
        }
    }
    std::vector<float> means(6);
    for (int i = 0; i < peaksNew.size(); i++) {
        float sum = std::accumulate(peaksNew[i].begin(), peaksNew[i].end(), 0.f,[](float acc, const Peak p) { return acc + p.mag; });
        means[i] = sum / peaksNew[i].size();
    }
    for(int band = 0; band < numBands; band++){
        float currMean = means[band] * 0.9f;
        auto& vec = peaksNew[band];

        vec.erase(
            std::remove_if(vec.begin(), vec.end(), [currMean](const Peak& v) {
                return v.mag <= currMean;
            }),
            vec.end()
        );
    }
    size_t total = 0;
    for (auto& v : peaksNew) total += v.size();

    std::vector<Peak> allPeaks;
    allPeaks.reserve(total);

    for (auto& bandPeaks : peaksNew)
        allPeaks.insert(allPeaks.end(), bandPeaks.begin(), bandPeaks.end());

    return allPeaks;

    //  for(int i = 0; i < spectrogram.size();i++){
    //      double curTime = i*hopTime;
    //
    //      //each band max
    //      for(int band = 0; band < numBands;band++){
    //          int startBin = ranges[band] + (band == 0 ? 1 : 0);
    //          int endBin = ranges[band + 1];
    //
    //          Peak bandPeak = {static_cast<float>(curTime), 0, 0.0f};
    //
    //          for(int bin = startBin; bin < endBin;bin++){
    //              if(spectrogram[i][bin] > bandPeak.mag){
    //                  bandPeak.bin = bin;
    //                  bandPeak.mag = spectrogram[i][bin];
    //              }
    //          }
    //
    //          if (bandPeak.mag > 0.f) {
    //              peaks.push_back(bandPeak);
    //          }
    //      }
    //  }
    //
    //  float sum = std::accumulate(peaks.begin(), peaks.end(), 0.0f,
    //                              [](float acc, const Peak p) { return acc + p.mag; });
    //  float mean = sum / static_cast<float>(peaks.size());
    //  mean = mean * 1.1f;
    //  std::cout << "Mean: " << mean << std::endl;
    //  std::cout << "Peaks before: " << peaks.size() << std::endl;
    //
    //
    //  std::erase_if(peaks,
    //                [mean](Peak v) { return v.mag <= mean; });
    //
    //  std::cout << "Peaks after: " << peaks.size() << std::endl;
    //
    // return peaks;
};


void wav::applyHammingWindow(std::vector<float> &window) {
    for (int i = 0; i < window_size; ++i) {
        window[i] *= 0.54 - 0.46 * cos(2 * M_PI * i / (window_size - 1));
    }
}

std::unordered_map<uint32_t, std::vector<double> > wav::createFingerprints(std::vector<wav::Peak> &peaks) {
    std::unordered_map<uint32_t, std::vector<double>> fingerprints;
    int targetZoneSize = 5;
    int anchorOffset = 2;

    int stopIdx = static_cast<int>(peaks.size()) - targetZoneSize - anchorOffset;

    for (int i = 0; i < stopIdx; i++) {
        const auto &anchor = peaks[i];
        double anchorTime = anchor.time;

        // Fixed target window
        // The shift in TGZ's brings huge success. larger time deltas could bring less false positives probably, and create more distinct fingerprints and depend on stronger context..
        for (int pointNum = 3; pointNum < targetZoneSize + 3; pointNum++) {
            const auto &target = peaks[i + pointNum];
            float deltaTime = target.time - anchor.time;

            // uint32_t address = (static_cast<uint32_t>(anchor.bin) << 23) |
            //                    (static_cast<uint32_t>(target.bin) << 14) |
            //                    (static_cast<uint32_t>(std::lround(deltaTime / 0.001f)) & 0x3FFF);

            uint32_t address = encoding::encode(anchor.bin, target.bin, deltaTime);

            fingerprints[address].push_back(anchorTime);
        }
    }

    std::cout << "Created " << fingerprints.size() << " unique fingerprints out of " <<peaks.size()<<" peaks" <<std::endl;
    return fingerprints;
}

/*
@param matches is of type [song_id] = {hash_x, a_time_x, ...}
@param clips is of type [hash] = {a_time1, a_time2, ...}

@return song_id of the matches song
 */
struct FingerprintValue {
    uint32_t hash;
    double anchor_time;
};

wav::Score wav::scoreMatches(
    std::unordered_map<uint32_t, std::vector<std::pair<int, double>>>& db_matches,
    std::unordered_map<uint32_t, std::vector<double>>& query_fingerprints
) {
    /// db matches holds **hash -> songId, occurenceTime** relation.

    wav::Score topScore = {-1, 0, 0};

    const int MIN_TGZ = 5;
    const float COHERENT_COEF = 0.05f; // 5% threshold


    // Step 1: Get TGZ Quantities - Count fingerprint occurrences
    std::cout << "\nCalculating TGZ quantities..." << std::endl;
    std::map<std::pair<int, double>, int> song_value_quantities;

    for (auto& [hash, query_times] : query_fingerprints) {
        if (db_matches.find(hash) == db_matches.end()) continue;

        for (auto& [song_id, db_time] : db_matches[hash]) {
            auto key = std::make_pair(song_id, db_time);
            song_value_quantities[key]++;
        }
    }

    std::cout << "Found " << song_value_quantities.size() << " distinct song values in query" << std::endl;

    // Step 2: Filter Songs - Keep only songs with strong TGZ
    std::cout << "\nFiltering songs by TGZ strength..." << std::endl;
    std::unordered_map<int, std::vector<FingerprintValue>> filtered_songs;
    std::unordered_map<int, int> song_couple_counts;
    std::unordered_map<int, int> song_tgz_counts;

    for (auto& [hash, query_times] : query_fingerprints) {
        if (db_matches.find(hash) == db_matches.end()) continue;

        for (auto& [song_id, db_time] : db_matches[hash]) {
            if (song_couple_counts.find(song_id) == song_couple_counts.end()) {
                song_couple_counts[song_id] = 0;
            }
            song_couple_counts[song_id]++;

            auto key = std::make_pair(song_id, db_time);
            int tgz_strength = song_value_quantities[key];

            // Only keep if TGZ is strong enough
            if (tgz_strength >= MIN_TGZ) {
                if (song_tgz_counts.find(song_id) == song_tgz_counts.end()) {
                    song_tgz_counts[song_id] = 0;
                }
                song_tgz_counts[song_id]++;

                if (filtered_songs.find(song_id) == filtered_songs.end()) {
                    filtered_songs[song_id] = std::vector<FingerprintValue>();
                }
                filtered_songs[song_id].push_back({hash, db_time});
            }
        }
    }

    // Step 3: Remove low-quality songs
    std::cout << "\nRemoving low-quality songs..." << std::endl;
    std::vector<int> songs_to_remove;

    for (auto& [song_id, fingerprints] : filtered_songs) {
        int tgz_count = song_tgz_counts[song_id];
        int couple_count = song_couple_counts[song_id];

        double tgz_ratio = couple_count > 0 ? (double)tgz_count / couple_count : 0;

        std::cout << "Song " << song_id << ": " << couple_count << " couples, "
                  << tgz_count << " in TGZ (" << (tgz_ratio * 100) << "%)" << std::endl;

        if ((tgz_count < 1400 || tgz_ratio < 0.3f) && tgz_ratio < 0.8f) {
            songs_to_remove.push_back(song_id);
            std::cout << "  → Removing (low quality)" << std::endl;
        }
    }

    for (int song_id : songs_to_remove) {
        filtered_songs.erase(song_id);
    }

    std::cout << filtered_songs.size() << " songs remain after filtering" << std::endl;

    std::unordered_map<int, std::unordered_map<long, int>> song_deltas;

    // For each hash that was queried
    for (auto& [hash, query_times] : query_fingerprints) {
        if (db_matches.find(hash) == db_matches.end()) continue;

        // For each database entry with this hash
        for (auto& [song_id, db_time] : db_matches[hash]) {
            if (filtered_songs.find(song_id) == filtered_songs.end()) continue;

            // Calculate offset for each query time with this hash
            for (double query_time : query_times) {
                long delta_ms = static_cast<long>((db_time - query_time) * 1000);

                if (song_deltas[song_id].find(delta_ms) == song_deltas[song_id].end()) {
                    song_deltas[song_id][delta_ms] = 0;
                }
                song_deltas[song_id][delta_ms]++;
            }
        }
    }

    // Step 5: Find best offset for each song

    int total_query_notes = 0;
    for (auto& [hash, times] : query_fingerprints) {
        total_query_notes += times.size();
    }
    std::cout << "Total query notes: " << total_query_notes << std::endl;

    for (auto& [song_id, delta_histogram] : song_deltas) {
        // Find delta with most occurrences
        long best_delta = 0;
        int max_coherent_notes = 0;

        for (auto& [delta, count] : delta_histogram) {
            if (count > max_coherent_notes) {
                max_coherent_notes = count;
                best_delta = delta;
            }
        }

        // Calculate coherency
        int total_matches = 0;
        for (auto& [delta, count] : delta_histogram) {
            total_matches += count;
        }

        double coherency_ratio = total_matches > 0 ?
            (double)max_coherent_notes / total_matches : 0;
        double percentage_of_query = (double)max_coherent_notes / total_query_notes * 100;

        std::cout << "Song " << song_id << ": " << max_coherent_notes
                  << " coherent notes (offset: " << (best_delta / 1000.0) << "s, "
                  << "coherency(max coherency in one go): " << (coherency_ratio * 100) << "%, "
                  << percentage_of_query << "% matching to all notes)" << std::endl;

        bool passes_percentage = max_coherent_notes >= COHERENT_COEF * total_query_notes;

        if (passes_percentage) {
            std::cout << "  ACCEPTED" << std::endl;

            if (max_coherent_notes > topScore.score) {
                topScore.songId = song_id;
                topScore.offset = best_delta / 1000.0f;  // Convert to seconds
                topScore.score = max_coherent_notes;
            }
        } else {
            std::cout << "   REJECTED: ";
            if (!passes_percentage) std::cout << "low_percentage ";
            std::cout << std::endl;
        }
    }

    std::cout << "\n======" << std::endl;
    if (topScore.songId != -1) {
        std::cout << "MATCH!" << std::endl;
        std::cout << "Song ID: " << topScore.songId << std::endl;
        std::cout << "Offset: " << topScore.offset << "s" << std::endl;
        std::cout << "Score: " << topScore.score << " coherent notes" << std::endl;
    } else {
        std::cout << "NO MATCH FOUND (no songs passed thresholds)" << std::endl;
    }

    return topScore;
}

/*Function that calls hashing on every anchor point and  */
void wav::processPeaks(std::vector<Peak> &peaks, bool toStore, const std::string &songName) {
    sqlite3_db db("store.db");

    auto fingerPrints = createFingerprints(peaks);

    if (toStore) {
        int song_id = db.db_insert_song(songName);
        db.db_process_fingerPrints(fingerPrints, song_id);
    } else {
        std::unordered_map<uint32_t, std::vector<std::pair<int, double> > > matches = db.db_match_fingerPrints(
            fingerPrints);
        auto result = scoreMatches(matches, fingerPrints);

        std::cout << "Offset: " << result.offset << "s" <<
                ", Song Id: " << result.songId <<
                ", Score: " << result.score << std::endl;
    }
}
