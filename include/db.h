//
// Created by obbte on 06/25/25.
//

#ifndef DB_H
#define DB_H

#include "sqlite3.h"
#include <string>
#include <vector>
#include "wavProcessing.h"


class sqlite3_db {
public:
    explicit sqlite3_db(const std::string &db_name);

    int db_insert_hash(uint32_t hash, int song_id, float anchor_time) const;

    int db_insert_song(const std::string &song_name) const;

    int db_create();

    int drop_db(int drop_code);

    int db_process_peaks(std::vector<wav::Peak> &peaks, int &song_id);

    ~sqlite3_db();

private:
    sqlite3 *_db{};

    int find_song_id(const std::string &song_name) const;
};


#endif //DB_H
