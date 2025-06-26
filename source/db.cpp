//
// Created by obbte on 06/25/25.
//

#include <iostream>
#include "../include/db.h"


sqlite3_db::sqlite3_db(const std::string &db_name) {
    auto rc = sqlite3_open(db_name.c_str(), &_db);

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(_db));
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }
}

sqlite3_db::~sqlite3_db() {
    sqlite3_close(_db);
}

bool sqlite3_db::song_exists(const std::string &song_name) const {
    const char *sql = "SELECT * FROM SONGS WHERE SONG_NAME = ?;";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Could not prepare SELECT: " << sqlite3_errmsg(_db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, song_name.c_str(), -1, SQLITE_TRANSIENT);

    // execute
    auto rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_ROW) {
        std::cerr << "The song does not exists: " << sqlite3_errmsg(_db) << std::endl;
        return false;
    }

    return true;
}


int sqlite3_db::db_insert_song(const std::string &song_name) const {
    // prepare
    if (song_exists(song_name)) {
        std::cerr << "Song already exists: " << song_name << std::endl;
        // TODO do not insert song but give the row!
    }
    const char *sql = "INSERT INTO SONGS (SONG_NAME) VALUES (?);";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Could not prepare INSERT: " << sqlite3_errmsg(_db) << std::endl;
        return -1;
    }

    sqlite3_bind_text(stmt, 1, song_name.c_str(), -1, SQLITE_TRANSIENT);

    // execute
    auto rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "Error inserting song: " << sqlite3_errmsg(_db) << std::endl;
        return -1;
    }

    // fetch new ID
    auto newId = sqlite3_last_insert_rowid(_db);
    std::cout << "Inserted: " << song_name << " with id: " << newId << std::endl;

    return static_cast<int>(newId);
}


int sqlite3_db::db_insert_hash(const uint32_t hash, int song_id, float anchor_time) const {
    const char *sql = "INSERT INTO FINGERPRINTS (HASH, SONG_ID, ANCHOR_TIME) VALUES (?,?,?);";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Could not prepare INSERT: " << sqlite3_errmsg(_db) << std::endl;
        return -1;
    }

    sqlite3_bind_int(stmt, 1, static_cast<int>(hash));
    sqlite3_bind_int(stmt, 1, song_id);
    sqlite3_bind_double(stmt, 1, anchor_time);


    // execute
    auto rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "Error inserting hash: " << sqlite3_errmsg(_db) << std::endl;
        return -1;
    }
    std::cout << "Inserted hash: " << hash << " anchor time: " << anchor_time << " song id: " << song_id << std::endl;

    return 0;
}

int sqlite3_db::drop_db(int drop_code) {
    const char *sql = nullptr;
    switch (drop_code) {
        case 0:
            sql = "DROP TABLE IF EXISTS SONGS; ";
            break;
        case 1:
            sql = "DROP TABLE IF EXISTS FINGERPRINTS; ";
            break;
        case 2:
            sql = "DROP TABLE IF EXISTS SONGS; DROP TABLE IF EXISTS FINGERPRINTS; ";
            break;
        default:
            std::cerr << "Unknown drop code: " << drop_code << std::endl;
    }
    if (sql == nullptr) return -1;
    char *messageError;

    auto rc = sqlite3_exec(_db, sql, nullptr, nullptr, &messageError);
    if (rc != SQLITE_OK) {
        std::cerr << "Error deleting table(s)" << std::endl;
        sqlite3_free(messageError);
        return 1;
    }
    std::cout << "Table(s) deleted successfully" << std::endl;

    return 0;
}

int sqlite3_db::db_create() {
    std::string table1 = "CREATE TABLE IF NOT EXISTS SONGS("
            "SONG_ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "SONG_NAME TEXT NOT NULL);";

    std::string table2 = "CREATE TABLE IF NOT EXISTS  FINGERPRINTS("
            "HASH INTEGER NOT NULL,"
            "SONG_ID INTEGER NOT NULL,"
            "ANCHOR_TIME REAL NOT NULL,"
            "PRIMARY KEY (HASH, SONG_ID, ANCHOR_TIME),"
            "FOREIGN KEY (SONG_ID) REFERENCES SONGS(SONG_ID));";

    char *messageError;


    int exit1 = sqlite3_exec(_db, table1.c_str(), nullptr, nullptr, &messageError);
    int exit2 = sqlite3_exec(_db, table2.c_str(), nullptr, nullptr, &messageError);

    if (exit1 != SQLITE_OK) {
        std::cerr << "Error Create Table 1" << std::endl;
        sqlite3_free(messageError);
        return 1;
    }
    std::cout << "Table 1 created Successfully" << std::endl;
    if (exit2 != SQLITE_OK) {
        std::cerr << "Error Create Table 2" << std::endl;
        sqlite3_free(messageError);
        return 1;
    }
    std::cout << "Table 2 created Successfully" << std::endl;

    return 0;
}
