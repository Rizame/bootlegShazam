#define DR_WAV_IMPLEMENTATION
#include "wavProcessing.h"

bool processFile(const char *fileName){
    drwav wav;
    if (!drwav_init_file(&wav, fileName, NULL)) {
        std::cerr << "Failed to open WAV file.\n";
        return false;
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

    return true;
}


