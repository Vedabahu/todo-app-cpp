#include "user_repository.hpp"
#include <sqlite3.h>
#include <stdexcept>

UserRepository::UserRepository(const Database &db) : database_(db)
{
}

bool UserRepository::usernameExists(const std::string &username)
{
    sqlite3 *db = database_.openConnection();

    const char *sql = "SELECT 1 FROM users WHERE username = ? LIMIT 1;";

    sqlite3_stmt *stmt = nullptr;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    bool exists = (rc == SQLITE_ROW);

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return exists;
}

void UserRepository::createUser(const User &user)
{
    sqlite3 *db = database_.openConnection();

    const char *sql = "INSERT INTO users (id, username, password_hash) "
                      "VALUES (?, ?, ?);";

    sqlite3_stmt *stmt = nullptr;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, user.id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, user.username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, user.password_hash.c_str(), -1, SQLITE_TRANSIENT);

    sqlite3_step(stmt);

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

std::optional<User> UserRepository::findByUsername(const std::string &username)
{
    sqlite3 *db = database_.openConnection();

    const char *sql = "SELECT id, username, password_hash "
                      "FROM users WHERE username = ? LIMIT 1;";

    sqlite3_stmt *stmt = nullptr;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        sqlite3_close(db);
        throw std::runtime_error("Failed to prepare statement");
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_ROW)
    {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return std::nullopt;
    }

    User user;

    user.id = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));

    user.username = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));

    user.password_hash = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return user;
}

std::optional<User> UserRepository::findById(const std::string &id)
{
    sqlite3 *db = database_.openConnection();

    const char *sql = "SELECT id, username, password_hash "
                      "FROM users WHERE id = ? LIMIT 1;";

    sqlite3_stmt *stmt = nullptr;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        sqlite3_close(db);
        throw std::runtime_error("Failed to prepare statement");
    }

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_ROW)
    {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return std::nullopt;
    }

    User user;

    user.id = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));

    user.username = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));

    user.password_hash = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return user;
}