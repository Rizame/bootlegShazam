//
// Created by risame on 11/19/25.
//

#ifndef SHAZAM_RECOGNISER_H
#define SHAZAM_RECOGNISER_H

#endif //SHAZAM_RECOGNISER_H

#include <iostream>
#include "../include/wavProcessing.h"
#include "../include/db.h"

class Recogniser {
private:
    sqlite3_db db;

public:
    Recogniser() : db("store.db") {
    }

    void insert_all_songs();
    void insert_one_song(const std::string &song_name);
    void recognize_audio(const std::string &song_name);
};


#endif //SHAZAM_CPPSERVER_H