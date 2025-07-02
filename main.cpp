#include <iostream>
#include "include/wavProcessing.h"
#include "include/db.h"
#include "matplotlibcpp.h"
#include "include/encoding.h"

void insert_all_songs() {
    sqlite3_db db("store.db");
    db.drop_db(2);
    db.db_create();
    std::vector<std::string> songs = {
        "NeverGonnaGive",
        "TogetherForever",
        "Casino",
        "fakeId",
        "ohWhou",
        "soldOut"
    };

    for (int i = 0; i < songs.size(); i++) {
        std::string song = songs[i];

        std::string filename = "songs/original/" + song + ".wav";
        std::vector samplesInput = wav::processFile(filename.c_str());
        std::vector<std::vector<float> > spectrogramInput = wav::createSpectrogram(samplesInput);


        auto peaksInput = wav::filterPeaks(spectrogramInput);
        wav::processPeaks(peaksInput, true, song);
        std::cout << std::endl;
    }
}

void process_trimmed_song(const std::string &song) {
    std::string filename = "songs/clips/" + song + ".wav";

    std::vector samplesInput = wav::processFile(filename.c_str());
    std::vector<std::vector<float> > spectrogramInput = wav::createSpectrogram(samplesInput);


    auto peaksInput = wav::filterPeaks(spectrogramInput);
    wav::processPeaks(peaksInput, false, song);
}


int main() {
    // insert_all_songs();

    process_trimmed_song("soldOutRec");

    return 0;
}
