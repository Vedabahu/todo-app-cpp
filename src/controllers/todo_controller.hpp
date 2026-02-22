#pragma once

#include <crow.h>
#include <string>
#include <vector>

#include "../middleware/auth_middleware.hpp"
#include "../models/todo.hpp"
#include "../repositories/todo_repository.hpp"
#include "../utils/uuid.hpp"

template <typename App> void register_todo_routes(App &app, TodoRepository &todoRepo)
{
    CROW_ROUTE(app, "/todos").methods(crow::HTTPMethod::Post)([&app, &todoRepo](const crow::request &req) {
        // Get authenticated user_id from middleware
        auto &ctx = app.template get_context<AuthMiddleware>(req);

        if (!ctx.user_id)
            return crow::response(401, "Unauthorized");

        auto body = crow::json::load(req.body);

        if (!body)
            return crow::response(400, "Invalid JSON");

        if (!body.has("title") || body["title"].t() != crow::json::type::String)
            return crow::response(400, "Missing or invalid title");

        std::string title = body["title"].s();

        if (title.empty())
            return crow::response(400, "Title cannot be empty");

        Todo todo;
        todo.id = generate_uuid();
        todo.user_id = *ctx.user_id;
        todo.title = title;
        todo.completed = false;

        todoRepo.createTodo(todo);

        crow::json::wvalue response;
        response["id"] = todo.id;
        response["title"] = todo.title;
        response["completed"] = todo.completed;

        return crow::response(201, response);
    });

    CROW_ROUTE(app, "/todos")([&app, &todoRepo](const crow::request &req) {
        auto &ctx = app.template get_context<AuthMiddleware>(req);

        if (!ctx.user_id)
            return crow::response(401, "Unauthorized");

        auto todos = todoRepo.getTodosByUserId(*ctx.user_id);

        crow::json::wvalue response = crow::json::wvalue::list();

        for (size_t i = 0; i < todos.size(); i++)
        {
            response[i]["id"] = todos[i].id;
            response[i]["title"] = todos[i].title;
            response[i]["completed"] = todos[i].completed;
        }

        return crow::response(response);
    });
}