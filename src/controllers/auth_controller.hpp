#pragma once
#include "../middleware/auth_middleware.hpp"
#include "../services/auth_service.hpp"
#include <crow.h>

template <typename App> void register_auth_routes(App &app, AuthService &authService, UserRepository &userRepo)
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

    CROW_ROUTE(app, "/me")
    ([&app, &userRepo](const crow::request &req) {
        auto &ctx = app.template get_context<AuthMiddleware>(req);

        if (!ctx.user_id)
            return crow::response(401, "Unauthorized");

        auto user = userRepo.findById(*ctx.user_id);

        if (!user)
            return crow::response(404, "User not found");

        crow::json::wvalue response;

        response["id"] = user->id;
        response["username"] = user->username;

        return crow::response(response);
    });
}