#include "database.hpp"
#include <stdexcept>

Database::Database(const std::string &path) : db_path_(path)
{
}

sqlite3 *Database::openConnection() const
{
    sqlite3 *db = nullptr;

    int rc = sqlite3_open(db_path_.c_str(), &db);
    if (rc != SQLITE_OK)
        throw std::runtime_error("Failed to open database");

    return db;
}

void Database::initialize()
{
    sqlite3 *db = openConnection();

    const char *sql = R"(
        PRAGMA journal_mode=WAL;

        CREATE TABLE IF NOT EXISTS users (
            id TEXT PRIMARY KEY,
            username TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL
        );

        CREATE TABLE IF NOT EXISTS todos (
            id TEXT PRIMARY KEY,
            user_id TEXT NOT NULL,
            title TEXT NOT NULL,
            completed INTEGER NOT NULL DEFAULT 0,
            FOREIGN KEY(user_id) REFERENCES users(id)
        );
    )";

    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK)
    {
        std::string error = errMsg;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        throw std::runtime_error("SQL error: " + error);
    }

    sqlite3_close(db);
}