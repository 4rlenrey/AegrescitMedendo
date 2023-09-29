#pragma once
#include <crow.h>
#include <regex>
#include "handler.hpp"

struct Authorization : crow::ILocalMiddleware
{
    struct context
    {
        std::string username{""};
        bool authenticated{false};
    };

    inline void before_handle(crow::request &req, crow::response &res, context &ctx)
    {
        // Verify JWT access token cookie and a username
        std::string username{""}, accessToken{""}, match{""}, cookieHeaderValue{req.get_header_value("Cookie")};
        std::regex regex("([^=;]+)=([^;]+)");

        // Extract key-value pairs from the cookie header
        auto words_begin = std::sregex_iterator(cookieHeaderValue.begin(), cookieHeaderValue.end(), regex);
        auto words_end = std::sregex_iterator();

        for (std::sregex_iterator i = words_begin; i != words_end; ++i)
        {
            std::smatch match = *i;
            std::string name = match.str(1);
            std::string value = match.str(2);
            // Check for access_token and username in the cookie
            if (utils::trim(name) == "access_token")
                accessToken = utils::trim(value);
            else if (utils::trim(name) == "username")
                username = utils::trim(value);
        }
        auto info{utils::verify_token(accessToken, username)};
        if (!info.correct)
        {
            if (info.error_inf == "token expired")
            {
                res.code = 302;
                res.redirect("/refresh?url=" + utils::encodeURIComponent(req.url)); // Check if it has been refreshed before tho
                res.end();
            }
            else
            {
                res.code = 401;
                res.redirect("/login");
                res.end();
            }
        }
        else
        {
            ctx.username = username;
            ctx.authenticated = true;
        }
    }
    inline void after_handle(crow::request &req, crow::response &res, context &ctx)
    {
    }
};
