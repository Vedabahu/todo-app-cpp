#include "auth_service.hpp"
#include "../utils/hashing.hpp"
#include "../utils/uuid.hpp"

#include <chrono>
#include <ctime>
#include <sstream>

AuthService::AuthService(UserRepository &repo) : repo_(repo)
{
}

bool AuthService::registerUser(const std::string &username, const std::string &password)
{
    if (username.empty() || password.empty())
        return false;

    if (repo_.usernameExists(username))
        return false;

    std::string password_hash = sha256(password);

    std::string user_id = generate_uuid();

    User user{user_id, username, password_hash};

    repo_.createUser(user);

    return true;
}