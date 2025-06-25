//
// Created by obbte on 06/25/25.
//

#ifndef DB_H
#define DB_H

#include "sqlite3.h"
#include <string>

class sqlite3_db {
public:
    explicit sqlite3_db(const std::string& db_name);

    ~sqlite3_db();

private:
    sqlite3 *_db{};
};


#endif //DB_H
