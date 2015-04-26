#include "DBManager.h"

DBManager::DBManager() : stmt(nullptr)
{    
    QFile dbfile("database.db");
    if(dbfile.exists())
    {
        sqlite3_open("database.db", &db);
    }
    else
    {
        sqlite3_open("database.db", &db);
        createTable();
    }
}

DBManager::~DBManager()
{
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void DBManager::createTable()
{
    sqlite3_prepare_v2(db,
                       "CREATE TABLE KeyHashes"
                       "("
                       "hash varchar(64)"
                       ");",
                       -1, &stmt, NULL);
    sqlite3_step(stmt);
    sqlite3_reset(stmt);
}

void DBManager::addHash(QByteArray &hash)
{
    sqlite3_prepare_v2(db, "INSERT INTO KeyHashes (hash)"
                           "VALUES (?1);",
                       -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, hash.data(), -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_reset(stmt);
}

bool DBManager::hashExists(QByteArray &hash)
{
    sqlite3_prepare(db, "SELECT hash "
                        "FROM KeyHashes "
                        "WHERE hash=?1",
                    -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, hash.data(), -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    const char *result = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
    bool b = result != nullptr;
    sqlite3_reset(stmt);
    return b;
}

