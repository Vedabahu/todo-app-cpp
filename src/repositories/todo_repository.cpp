#include <sqlite3.h>
#include <stdexcept>

#include "todo_repository.hpp"

TodoRepository::TodoRepository(const Database &db) : database_(db)
{
}

void TodoRepository::createTodo(const Todo &todo)
{
    sqlite3 *db = database_.openConnection();

    const char *sql = "INSERT INTO todos (id, user_id, title, completed) "
                      "VALUES (?, ?, ?, ?);";

    sqlite3_stmt *stmt = nullptr;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        sqlite3_close(db);
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    sqlite3_bind_text(stmt, 1, todo.id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, todo.user_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, todo.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, todo.completed ? 1 : 0);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

std::vector<Todo> TodoRepository::getTodosByUserId(const std::string &user_id)
{
    sqlite3 *db = database_.openConnection();

    const char *sql = "SELECT id, user_id, title, completed "
                      "FROM todos WHERE user_id = ?;";

    sqlite3_stmt *stmt = nullptr;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        sqlite3_close(db);
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    sqlite3_bind_text(stmt, 1, user_id.c_str(), -1, SQLITE_TRANSIENT);

    std::vector<Todo> todos;

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        Todo todo;

        todo.id = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));

        todo.user_id = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));

        todo.title = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));

        todo.completed = sqlite3_column_int(stmt, 3) != 0;

        todos.push_back(todo);
    }

    if (rc != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return todos;
}