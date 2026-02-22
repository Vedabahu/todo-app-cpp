#pragma once

#include "../services/auth_service.hpp"
#include <crow.h>

struct AuthMiddleware
{
    struct context
    {
        std::optional<std::string> user_id;
    };

    AuthService *auth_service = nullptr;

    AuthMiddleware() = default;

    void before_handle(crow::request &req, crow::response &res, context &ctx)
    {
        // Ignore for public route: /register
        if (req.url == "/register")
            return;

        if (!auth_service)
        {
            res.code = 500;
            res.write("Auth middleware not initialized");
            res.end();
            return;
        }

        auto maybe_user = auth_service->verifyBasicAuth(req);

        if (!maybe_user)
        {
            res.code = 401;
            res.set_header("WWW-Authenticate", "Basic realm=\"User Visible Realm\"");
            res.write("Unauthorized");
            res.end();
            return;
        }

        ctx.user_id = *maybe_user;
    }

    void after_handle(crow::request &, crow::response &, context &)
    {
    }
};