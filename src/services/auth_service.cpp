#include "auth_service.hpp"
#include "../utils/hashing.hpp"
#include "../utils/uuid.hpp"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <crow.h>
#include <ctime>
#include <optional>
#include <sstream>
#include <string>

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

std::optional<std::string> AuthService::verifyBasicAuth(const crow::request &req)
{
    std::string authorization = req.get_header_value("Authorization");
    // 7 because "basic X" is 7 characters long including some content
    if (authorization.empty() || authorization.length() < 7)
        return std::nullopt;

    // Not Basic Auth
    std::string first_part = authorization.substr(0, 6);
    std::transform(first_part.begin(), first_part.end(), first_part.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    if (first_part != "basic ")
        return std::nullopt;

    // Credentials checking
    auto trim = [](std::string &s) {
        const char *whitespace = " \t\n\r\f\v";
        s.erase(0, s.find_first_not_of(whitespace));
        s.erase(s.find_last_not_of(whitespace) + 1);
    };
    std::string credentials = authorization.substr(6);
    trim(credentials);

    if (credentials.empty())
        return std::nullopt;

    // base64decode returns something. If it is not possible to decode properly, it replaces it with a 0
    // This is okay as it will still showup as an error in the header.
    std::string d_credentials = crow::utility::base64decode(credentials);
    size_t found = d_credentials.find(":");

    if (found == std::string::npos)
        return std::nullopt;

    std::string username = d_credentials.substr(0, found);
    std::string password = d_credentials.substr(found + 1);

    if (username.empty() || password.empty())
        return std::nullopt;

    auto user = repo_.findByUsername(username);
    if (!user)
        return std::nullopt;

    std::string hash = sha256(password);

    if (user->password_hash == hash)
        return user->id;

    return std::nullopt;
}
