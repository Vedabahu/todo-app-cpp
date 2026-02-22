#include <crow.h>
#include <filesystem>
#include <string>

#include "controllers/auth_controller.hpp"
#include "controllers/todo_controller.hpp"
#include "database/database.hpp"
#include "middleware/auth_middleware.hpp"
#include "repositories/todo_repository.hpp"
#include "repositories/user_repository.hpp"
#include "services/auth_service.hpp"

int main()
{
    std::filesystem::create_directories("data");

    Database db("data/todo.db");
    db.initialize();

    UserRepository userRepo(db);
    TodoRepository todoRepo(db);
    AuthService authService(userRepo);

    crow::App<AuthMiddleware> app;
    app.get_middleware<AuthMiddleware>().auth_service = &authService;
    app.use_compression(crow::compression::algorithm::GZIP);

    register_auth_routes(app, authService, userRepo);
    register_todo_routes(app, todoRepo);

    CROW_ROUTE(app, "/add/<int>/<int>")
    ([](int a, int b) { return std::to_string(a + b); });

    // app.port(18080).multithreaded().run();
    app.port(18080).run();
}