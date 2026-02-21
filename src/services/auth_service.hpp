#pragma once

#include "../repositories/user_repository.hpp"
#include <string>

class AuthService
{
  public:
    explicit AuthService(UserRepository &repo);

    bool registerUser(const std::string &username, const std::string &password);

  private:
    UserRepository &repo_;
};