#pragma once

#include <crow.h>

#include "../repositories/user_repository.hpp"
#include <string>

class AuthService
{
  public:
    explicit AuthService(UserRepository &repo);

    bool registerUser(const std::string &username, const std::string &password);

    std::optional<std::string> verifyBasicAuth(const crow::request &req);

  private:
    UserRepository &repo_;
};