#include <crow.h>

int main() {
  crow::SimpleApp app;

  CROW_ROUTE(app, "/")([]() { return "Crow is working."; });

  app.port(18080).multithreaded().run();
}