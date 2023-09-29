#pragma once

#include <openssl/sha.h>
#include <memory>
#include <string>
#include <crow.h>
#include <vector>
#include <jwt/jwt.hpp>
#include <iomanip>
#include <sstream>

#include "../external/sqlite_orm/include/sqlite_orm/sqlite_orm.h"

namespace utils
{
    struct TokenInfo
    {
        std::string error_inf;
        bool correct;

        TokenInfo() = default;
        TokenInfo(std::string inf, bool cr) : error_inf(inf), correct(cr) {};
        TokenInfo(const TokenInfo &) = default;
        ~TokenInfo() = default;
    };

    const std::string SECRET_KEY = "my_secret_key"; // To be changed. Maybe use RSA?
    const std::string ALGORITHM = "HS256";
    const std::string REFRESH = "refresh";
    const std::string ACCESS = "access";
    const int ACCESS_TOKEN_LIFESPAN = 15;
    const int REFRESH_TOKEN_LIFESPAN = 21600; // 15 days
    
    std::string sha256(const std::string &str);
    std::string generate_token(const std::string &username, const std::string &type);
    TokenInfo verify_token(const std::string &token, const std::string &expected_username);
    std::string trim(const std::string &str);
    std::string encodeURIComponent(const std::string& str);
    std::string decodeURIComponent(const std::string& str);
}
