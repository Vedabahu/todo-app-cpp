#pragma once
#include "../services/auth_service.hpp"
#include <crow.h>

void register_auth_routes(crow::Crow<> &app, AuthService &authService);