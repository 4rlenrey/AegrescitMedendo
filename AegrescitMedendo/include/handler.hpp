#pragma once
#include "storage.hpp"
#include <crow.h>

typedef crow::mustache::rendered_template page;
typedef crow::response resp;

namespace Rhandlers
{
    resp refresh_token(const crow::request &req, std::shared_ptr<Storage> &storage);
    resp obtain_token(const crow::request &req, std::shared_ptr<Storage> &storage);
    resp perform_transaction(const crow::request &req, std::shared_ptr<Storage> &storage, std::string username);
};

namespace Pages
{
    page admin(std::shared_ptr<Storage> &storage);
    page main(const std::string &username, std::shared_ptr<Storage> &storage);
    page user(const std::string &username, const int &id, std::shared_ptr<Storage> &storage);
    page p404();
    page login();
    page about();
    page refresh(const crow::request &req);
    page product(const std::string &username, std::shared_ptr<Storage> &storage, const int &id);
};

namespace CRUDns
{

    inline crow::response create_product(const crow::request &req, std::shared_ptr<Storage> &storage,
                                         const std::string &seller_username)
    {
        crow::response res;
        crow::json::rvalue jsonValue;
        try
        {
            jsonValue = crow::json::load(req.body);
        }
        catch (const std::exception &e)
        {
            res.code = 400;
            res.body = "Failed to parse JSON: " + std::string(e.what());
            return res;
        }
        try
        {
            crow::json::wvalue targetValue = std::move(jsonValue);
            auto seller = storage->get_user(seller_username);
            targetValue["seller_id"] = seller.id;
            storage->create_object<Product>(crow::json::load(targetValue.dump()));

            res.code = 200;
            res.body = "Created object";
        }
        catch (const std::exception &e)
        {
            res.code = 400;
            res.body = "Couldn't create that object: " + std::string(e.what());
            return res;
        }
        return res;
    };

    template <typename TypeClass>
    inline crow::response create_object_handler(const crow::request &req, std::shared_ptr<Storage> &storage)
    {
        crow::response res;
        crow::json::rvalue jsonValue;
        try
        {
            jsonValue = crow::json::load(req.body);
        }
        catch (const std::exception &e)
        {
            // seller_id
            res.code = 400;
            res.body = "Failed to parse JSON: " + std::string(e.what());
            return res;
        }
        try
        {
            storage->create_object<TypeClass>(jsonValue);

            res.code = 200;
            res.body = "Created object";
        }
        catch (const std::exception &e)
        {
            res.code = 400;
            res.body = "Couldn't create that object: " + std::string(e.what());
            return res;
        }
        return res;
    };

    template <typename TypeClass>
    inline crow::response update_object_handler(const crow::request &req, std::shared_ptr<Storage> &storage, const int &id)
    {
        crow::response res;
        crow::json::rvalue jsonValue;
        try
        {
            jsonValue = crow::json::load(req.body);
        }
        catch (const std::exception &e)
        {
            res.code = 400;
            res.body = "Failed to parse JSON: " + std::string(e.what());
            return res;
        }
        try
        {
            storage->update_object<TypeClass>(id, jsonValue);
            res.code = 200;
            res.body = "Created object";
        }
        catch (const std::exception &e)
        {
            res.code = 400;
            res.body = "Couldn't update that object: " + std::string(e.what());
            return res;
        }
        return res;
    };

    template <typename TypeClass>
    inline crow::response get_objects(std::shared_ptr<Storage> &storage)
    {
        return storage->get_jsons<TypeClass>();
    }

    template <typename TypeClass>
    inline crow::response delete_object(const int &id, std::shared_ptr<Storage> &storage)
    {
        crow::response res;
        try
        {
            storage->delete_object<TypeClass>(id);
            res.code = 200;
            res.body = "Created object";
        }
        catch (const std::exception &e)
        {
            res.code = 400;
            res.body = "Couldn't delete that object: " + std::string(e.what());
            return res;
        }
        return res;
    }

};