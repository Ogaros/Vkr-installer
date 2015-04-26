#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "sqlite3.h"
#include <qbytearray.h>
#include <qfile.h>

class DBManager
{
public:
    DBManager();
    ~DBManager();    
    void addHash(QByteArray &hash);
    bool hashExists(QByteArray &hash);

private:
    void createTable();
    sqlite3 *db;
    sqlite3_stmt *stmt;
};

#endif // DBMANAGER_H
