//
// Created by obbte on 06/25/25.
//

#include "../include/db.h"


sqlite3_db::sqlite3_db(const std::string& db_name) {
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

