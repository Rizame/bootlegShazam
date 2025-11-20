//
// Created by risame on 11/19/25.
//
#include "../include/recogniser.h"

void Recogniser::insert_all_songs() {
    db.drop_db(2);
    db.db_create();
    std::vector<std::string> songs = {
        "NeverGonnaGive",
        "TogetherForever",
        "Casino",
        "fakeId",
        "ohWhou",
        "soldOut",
        "cheba",
        "psychoPac"
    };

    for (int i = 0; i < songs.size(); i++) {
        std::string song = songs[i];

        std::string filename = "songs/original/" + song + ".wav";
        std::cout<<filename<<"\n";
        std::flush(std::cout);
        std::vector samplesInput = wav::processFile(filename.c_str());
        std::vector<std::vector<float> > spectrogramInput = wav::createSpectrogram(samplesInput);


        auto peaksInput = wav::filterPeaks(spectrogramInput);
        wav::processPeaks(peaksInput, true, song);
        std::cout << std::endl;
    }
}

void Recogniser::insert_one_song(const std::string &song_name) {
    std::string filename = "songs/original/" + song_name + ".wav";
    std::vector samplesInput = wav::processFile(filename.c_str());
    std::vector<std::vector<float> > spectrogramInput = wav::createSpectrogram(samplesInput);

    auto peaksInput = wav::filterPeaks(spectrogramInput);
    wav::processPeaks(peaksInput, true, song_name);
    std::cout << std::endl;
}
void Recogniser::recognize_audio(const std::string &song_name) {
    std::string filename = "songs/clips/" + song_name + ".wav";

    std::vector<float> samplesInput = wav::processFile(filename.c_str());
    //TODO check downsampling from 48000 to 44100 -------> maybe FFMPEG?

    wav::recreateSound(samplesInput);
    std::vector<std::vector<float> > spectrogramInput = wav::createSpectrogram(samplesInput);


    auto peaksInput = wav::filterPeaks(spectrogramInput);
    wav::processPeaks(peaksInput, false, song_name);
}