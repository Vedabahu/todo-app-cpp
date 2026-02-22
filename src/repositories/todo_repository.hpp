#pragma once

#include <vector>

#include "../database/database.hpp"
#include "../models/todo.hpp"

class TodoRepository
{
  public:
    explicit TodoRepository(const Database &db);
    void createTodo(const Todo &todo);
    std::vector<Todo> getTodosByUserId(const std::string &user_id);

  private:
    const Database &database_;
};