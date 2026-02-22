#pragma once
#include "../services/auth_service.hpp"
#include <crow.h>

template <typename App> void register_auth_routes(App &app, AuthService &authService)
{
    CROW_ROUTE(app, "/register").methods(crow::HTTPMethod::Post)([&authService](const crow::request &req) {
        auto body = crow::json::load(req.body);

        if (!body)
            return crow::response(400, "Invalid JSON");

        if (!body.has("username") || !body.has("password"))
            return crow::response(400, "Missing fields");

        std::string username = body["username"].s();
        std::string password = body["password"].s();

        if (username.empty() || password.empty())
            return crow::response(400, "Empty username or password");

        bool success = authService.registerUser(username, password);

        if (!success)
            return crow::response(409, "User exists");

        return crow::response(201, "User created");
    });
}