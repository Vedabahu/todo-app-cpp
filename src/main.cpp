#include <crow.h>
#include <filesystem>
#include <string>

int main()
{
    std::filesystem::path source_file = __FILE__;
    std::filesystem::path source_dir = source_file.parent_path();
    std::filesystem::path templates_path = source_dir / "templates";

    crow::App app;
    app.use_compression(crow::compression::algorithm::GZIP);
    crow::mustache::set_global_base(templates_path.string());

    CROW_ROUTE(app, "/")([]() {
        crow::mustache::template_t page = crow::mustache::load("hello.html");
        return page.render();
    });

    CROW_ROUTE(app, "/hello/<string>")([](std::string name) {
        crow::mustache::context ctx;
        ctx["name"] = name;
        crow::mustache::template_t page = crow::mustache::load("hello_name.html");
        return page.render(ctx);
    });

    app.port(18080).multithreaded().run();
}