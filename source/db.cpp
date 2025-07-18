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

int sqlite3_db::find_song_id(const std::string &song_name) const {
    const char *sql = "SELECT * FROM SONGS WHERE SONG_NAME = ?;";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Could not prepare SELECT: " << sqlite3_errmsg(_db) << std::endl;
        return -1;
    }

    sqlite3_bind_text(stmt, 1, song_name.c_str(), -1, SQLITE_TRANSIENT);

    // execute
    auto rc = sqlite3_step(stmt);
    int id = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE) {
        std::cerr << "The song does not exist: " << song_name << std::endl;
        return -1;
    }

    return id;
}


int sqlite3_db::db_insert_song(const std::string &song_name) const {
    // prepare
    auto song = find_song_id(song_name);
    if (song != -1) {
        std::cerr << "Song already exists: " << song_name << std::endl;
        return song;
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
    sqlite3_bind_int(stmt, 2, song_id);
    sqlite3_bind_double(stmt, 3, anchor_time);


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

int sqlite3_db::db_process_fingerPrints(std::unordered_map<uint32_t, std::vector<double> > &fingerprints, int &song_id) {
    char *messageError;

    auto rc = sqlite3_exec(_db, "BEGIN TRANSACTION;", nullptr, nullptr, &messageError);

    if (rc != SQLITE_OK) {
        std::cerr << "Error begin transaction" << std::endl;
        sqlite3_free(messageError);
        return -1;
    }

    const char *sql = "INSERT INTO FINGERPRINTS (HASH, SONG_ID, ANCHOR_TIME) VALUES (?,?,?);";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Could not prepare INSERT: " << sqlite3_errmsg(_db) << std::endl;
        return -1;
    }

    for (const auto &[hash, times]: fingerprints) {
        for (const auto &time: times) {
            sqlite3_reset(stmt);
            sqlite3_bind_int(stmt, 1, hash);
            sqlite3_bind_int(stmt, 2, song_id);
            sqlite3_bind_double(stmt, 3, time);

            if (sqlite3_step(stmt) != SQLITE_DONE) {
                std::cerr << "insert failed: " << sqlite3_errmsg(_db) << std::endl;
            }
        }
    }


    sqlite3_finalize(stmt);
    sqlite3_exec(_db, "COMMIT;", nullptr, nullptr, &messageError);

    return 0;
}

std::unordered_map<uint32_t, std::vector<std::pair<int, double> > > sqlite3_db::db_match_fingerPrints(
    std::unordered_map<uint32_t, std::vector<double> > &fingerPrints) {
    std::string paramList;

    int done = 0;
    int size = fingerPrints.size() - 1;

    for (const auto &[hash, times]: fingerPrints) {
        paramList.append("?");
        if (done < size) {
            paramList.append(", ");
        }
        done++;
    }


    std::string sql = "SELECT * FROM FINGERPRINTS WHERE HASH IN (" + paramList + ");";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(_db) << "\n";
        return {};
    }

    int i = 0;
    for (const auto &[hash, times]: fingerPrints) {
        sqlite3_bind_int(stmt, i + 1, hash);
        i++;
    }

    // hash map -> [song] = <vector>{hash, anchor_time}
    std::unordered_map<uint32_t, std::vector<std::pair<int, double> > > song_fingerprints;


    while (sqlite3_step(stmt) == SQLITE_ROW) {
        auto hash = static_cast<uint32_t>(sqlite3_column_int(stmt, 0));
        int song_id = sqlite3_column_int(stmt, 1);
        double anchor_time = sqlite3_column_double(stmt, 2);
        song_fingerprints[song_id].emplace_back(hash, anchor_time);
    }


    for (const auto &[song_id, vec]: song_fingerprints) {
        std::cout << "Potential songs: " << song_id << " with matches: " << vec.size() << std::endl;
    }

    sqlite3_finalize(stmt);
    return song_fingerprints;
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
