#include "../include/utils.hpp"

std::string utils::sha256(const std::string &str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    std::string hashed_str;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        hashed_str += hash[i];
    return hashed_str;
}

std::string utils::generate_token(const std::string &username, const std::string &type)
{
    jwt::jwt_object obj{jwt::params::algorithm(utils::ALGORITHM), jwt::params::secret(utils::SECRET_KEY)};

    // Imprint your identity upon the token, a testament to your existence in this vast cosmic tableau
    obj.add_claim("username", username);
    obj.add_claim("type", type);
    // Embrace the fleeting nature of time, capturing the essence of this moment in the realm of eternity
    obj.add_claim("iat", std::chrono::system_clock::now());

    auto time_addition = std::chrono::minutes{utils::ACCESS_TOKEN_LIFESPAN};
    if (type == "refresh")
        time_addition = std::chrono::minutes{utils::REFRESH_TOKEN_LIFESPAN};

    // Carve the expiration claim upon your token, as it marches towards its inevitable end
    obj.add_claim("exp", std::chrono::system_clock::now() + time_addition);

    return obj.signature();
}

utils::TokenInfo utils::verify_token(const std::string &token, const std::string &expected_username)
{
    try
    {
        // Push through the obstacles and decode the token with unwavering determination
        jwt::jwt_object decoded_token = jwt::decode(token, jwt::params::algorithms({utils::ALGORITHM}), jwt::params::secret(utils::SECRET_KEY));

        if (!decoded_token.payload().has_claim_with_value("username", expected_username))
            return TokenInfo{"Username does not match", false};

        // Embrace the present moment, as time is both your ally and your challenge
        auto now = std::chrono::system_clock::now();

        auto issued_at = decoded_token.payload().get_claim_value<uint64_t>("iat");
        auto exp = decoded_token.payload().get_claim_value<uint64_t>("exp");

        auto issued_at_time_point = std::chrono::system_clock::from_time_t(issued_at);
        auto exp_time_point = std::chrono::system_clock::from_time_t(exp);

        if (exp_time_point <= now || issued_at_time_point > now)
            return TokenInfo{"expired", false};

        return TokenInfo{"", true};
    }
    catch (const std::exception &ex)
    {
        return TokenInfo{std::string(ex.what()), false};
    }
}

std::string utils::trim(const std::string &str)
{
    const std::string &whitespace = " \t";
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return "";

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

std::string utils::encodeURIComponent(const std::string& str) {
    std::ostringstream encodedStream;
    encodedStream.fill('0');
    encodedStream << std::hex;

    for (char c : str) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encodedStream << c;
        } else {
            encodedStream << '%' << std::uppercase << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
        }
    }

    return encodedStream.str();
}

std::string utils::decodeURIComponent(const std::string& str) {
    std::ostringstream decodedStream;

    for (std::size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%') {
            int hexValue = 0;
            std::istringstream hexStream(str.substr(i + 1, 2));
            hexStream >> std::hex >> hexValue;
            decodedStream << static_cast<char>(hexValue);
            i += 2; // Skip the next two characters since they have been processed
        } else if (str[i] == '+') {
            decodedStream << ' ';
        } else {
            decodedStream << str[i];
        }
    }

    return decodedStream.str();
}