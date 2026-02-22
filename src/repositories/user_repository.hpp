#pragma once

#include "../database/database.hpp"
#include "../models/user.hpp"
#include <optional>

class UserRepository
{
  public:
    explicit UserRepository(const Database &db);

    bool usernameExists(const std::string &username);
    void createUser(const User &user);
    std::optional<User> findByUsername(const std::string &username);
    std::optional<User> findById(const std::string &id);

  private:
    const Database &database_;
};