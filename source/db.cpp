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

static int callback(void *data, int argc, char **argv, char **azColName) {
    int i;
    fprintf(stderr, "%s: ", (const char *) data);

    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    printf("\n");
    return 0;
}

int sqlite3_db::db_insert_hash(uint32_t hash) {
    std::string sql("INSERT INTO FINGERPRINTS VALUES(" + std::to_string(hash) + ");");

    int exit = 0;
    char *messaggeError;
    exit = sqlite3_exec(_db, sql.c_str(), NULL, 0, &messaggeError);

    if (exit != SQLITE_OK) {
        std::cerr << "Error Create Table" << std::endl;
        sqlite3_free(messaggeError);
        return 1;
    } else
        std::cout << "Table created Successfully" << std::endl;

    return 0;
}

int sqlite3_db::db_create() {
    std::string sql1 = "CREATE TABLE SONGS("
                       "SONG_ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                       "SONG_NAME TEXT NOT NULL);";
    std::string sql2 = "CREATE TABLE FINGERPRINTS("
                       "HASH INTEGER NOT NULL,"
                       "SONG_ID INTEGER NOT NULL,"
                       "TIMESTAMP FLOAT NOT NULL,"
                       "PRIMARY KEY (HASH, SONG_ID, TIMESTAMP),"
                       "FOREIGN KEY (SONG_ID) REFERENCES SONGS(SONG_ID));";

    char *messaggeError;
    int exit1 = sqlite3_exec(_db, sql1.c_str(), NULL, 0, &messaggeError);
    int exit2 = sqlite3_exec(_db, sql2.c_str(), NULL, 0, &messaggeError);

    if (exit1 != SQLITE_OK) {
        std::cerr << "Error Create Table 1" << std::endl;
        sqlite3_free(messaggeError);
        return 1;
    } else
        std::cout << "Table 1 created Successfully" << std::endl;
    if (exit2 != SQLITE_OK) {
        std::cerr << "Error Create Table 2" << std::endl;
        sqlite3_free(messaggeError);
        return 1;
    } else
        std::cout << "Table 2 created Successfully" << std::endl;

    return 0;
}
