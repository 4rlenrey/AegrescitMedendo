#pragma once
#include "classes.hpp"
#include <type_traits>
#include <initializer_list>

inline auto init_storage(std::string db_file = "AAAAAA.db")
{
    using namespace sqlite_orm;
    auto storage = make_storage(
        db_file,
        make_table("users",
                   make_column("id", &User::id, primary_key().autoincrement()),
                   make_column("name", &User::name),
                   make_column("surname", &User::surname),
                   make_column("email", &User::email),
                   make_column("phone", &User::phone),
                   make_column("username", &User::username),
                   make_column("password", &User::password),
                   make_column("balance", &User::balance, default_value(0.0))),
        make_table("products_bought",
                   make_column("id", &ProductBought::id, primary_key().autoincrement()),
                   make_column("buyer_id", &ProductBought::buyer_id),
                   make_column("product_id", &ProductBought::product_id),
                   make_column("amount", &ProductBought::amount),
                   make_column("price", &ProductBought::price, default_value(0.0)),
                   foreign_key(&ProductBought::buyer_id).references(&User::id),
                   foreign_key(&ProductBought::product_id).references(&Product::id)),
        make_table("categories",
                   make_column("id", &Category::id, primary_key().autoincrement()),
                   make_column("name", &Category::name)),
        make_table("products",
                   make_column("id", &Product::id, primary_key().autoincrement()),
                   make_column("title", &Product::title),
                   make_column("description", &Product::description),
                   make_column("amount", &Product::amount),
                   make_column("price", &Product::price),
                   make_column("seller_id", &Product::seller_id),
                   make_column("category_id", &Product::category_id),
                   foreign_key(&Product::seller_id).references(&User::id),
                   foreign_key(&Product::category_id).references(&Category::id)),
        make_table("reviews",
                   make_column("id", &Review::id, primary_key().autoincrement()),
                   make_column("user_id", &Review::user_id),
                   make_column("product_id", &Review::product_id),
                   make_column("rating", &Review::rating),
                   make_column("content", &Review::content),
                   foreign_key(&Review::user_id).references(&User::id),
                   foreign_key(&Review::product_id).references(&Product::id)));
    storage.sync_schema();

    return storage;
};

class Storage
{
private:
public:
    decltype(init_storage()) storage;
    Storage() : storage(init_storage()){};

    template <typename TypeClass>
    inline TypeClass get_object(int id)
    {
        return storage.get<TypeClass>(id);
    };

    template <typename TypeClass>
    inline crow::json::wvalue get_base_json(int id)
    {
        auto object = storage.get<TypeClass>(id);
        return object.as_json();
    };

    template <typename TypeClass>
    inline void fill_json_fields(crow::json::wvalue &object_json)
    {
        // TODO: Clean this up
        if (std::is_same<TypeClass, Product>::value)
        {
            auto user_json = std::move(this->get_base_json<User>(std::stoi(object_json["seller_id"].dump())));
            object_json["seller_name"] = std::move(user_json["name"]);
            object_json["seller_surname"] = std::move(user_json["surname"]);
            object_json["category"] = std::move(this->get_base_json<Category>(std::stoi(object_json["category_id"].dump()))["name"]);
        }
        else if (std::is_same<TypeClass, ProductBought>::value)
        {
            auto user_json = std::move(this->get_base_json<User>(std::stoi(object_json["buyer_id"].dump())));
            object_json["buyer_name"] = std::move(user_json["name"]);
            object_json["buyer_surname"] = std::move(user_json["surname"]);
            object_json["product"] = std::move(this->get_base_json<Product>(std::stoi(object_json["product_id"].dump()))["title"]);
        }
        else if (std::is_same<TypeClass, Review>::value)
        {
            auto user_json = std::move(this->get_base_json<User>(std::stoi(object_json["user_id"].dump())));
            object_json["user_name"] = std::move(user_json["name"]);
            object_json["user_surname"] = std::move(user_json["surname"]);
            object_json["product"] = std::move(this->get_base_json<Product>(std::stoi(object_json["product_id"].dump()))["title"]);
        }
        // TODO: Consider adding a list of children for User/Category (For example sold/bought products for user)
        // Nothing to fill out in Category, so it's ignored
        // Nothing to fill out in User, so it's ignored
    }

    template <typename TypeClass>
    inline crow::json::wvalue get_json(int id)
    {
        crow::json::wvalue object_json{get_base_json<TypeClass>(id)};
        fill_json_fields<TypeClass>(object_json);
        return object_json;
    };

    inline crow::json::wvalue get_review_jsons(int id)
    {
        std::vector<crow::json::wvalue> jsons;
        auto objects = storage.get_all<Review>();
        for (const auto &object : objects)
        {
            if (object.product_id != id)
                continue;

            crow::json::wvalue object_json = std::move(object.as_json());
            fill_json_fields<Review>(object_json);

            jsons.push_back(object_json);
        }
        crow::json::wvalue json_array(jsons);
        return json_array;
    };

    inline crow::json::wvalue get_orders(int buyer_id){
        std::vector<crow::json::wvalue> jsons;
        auto objects = storage.get_all<ProductBought>(sqlite_orm::where(sqlite_orm::c(&ProductBought::buyer_id) == buyer_id));
        for (const auto &object : objects)
        {
            crow::json::wvalue object_json = std::move(object.as_json());
            fill_json_fields<ProductBought>(object_json);

            jsons.push_back(object_json);
        }
        crow::json::wvalue json_array(jsons);
        return json_array;
    };


    template <typename TypeClass>
    inline crow::json::wvalue get_jsons()
    {
        std::vector<crow::json::wvalue> jsons;
        auto objects = storage.get_all<TypeClass>();
        for (const auto &object : objects)
        {
            crow::json::wvalue object_json = std::move(object.as_json());
            fill_json_fields<TypeClass>(object_json);

            jsons.push_back(object_json);
        }
        crow::json::wvalue json_array(jsons);
        return json_array;
    }

    inline crow::json::wvalue get_product_jsons(int user_id)
    {
        std::vector<crow::json::wvalue> jsons;
        // Maybe can be simplified with get_jsons?
        auto objects = storage.get_all<Product>(sqlite_orm::where(sqlite_orm::c(&Product::seller_id) == user_id));
        for (const auto &object : objects)
        {
            crow::json::wvalue object_json = std::move(object.as_json());
            fill_json_fields<Product>(object_json);

            jsons.push_back(object_json);
        }
        crow::json::wvalue json_array(jsons);
        return json_array;
    }

    User get_user(const std::string &username);

    template <typename TypeClass>
    inline void delete_object(int id)
    {
        storage.remove<TypeClass>(id);
    }

    template <typename TypeClass>
    inline void create_object(const crow::json::rvalue &jsonValue)
    {
        TypeClass object(jsonValue, 1);
        if (std::is_same<TypeClass, User>::value)
        {
            std::string username{jsonValue["username"].s()}; // Had to cast it to normal string for sqlite_orm
            auto userExists = storage.count<User>(sqlite_orm::where(
                sqlite_orm::c(&User::username) == username));
            if (userExists > 0)
            {
                return; // Do not create a user if a user with that username already exists
            }
        }
        storage.insert<TypeClass>(object);
    }

    template <typename TypeClass>
    inline void update_object(const int &id, const crow::json::rvalue &jsonValue)
    {
        TypeClass object(jsonValue, id);
        storage.update<TypeClass>(object);
    }

    template <typename TypeClass>
    inline void update_object(const TypeClass &obj)
    {
        storage.update<TypeClass>(obj);
    }


    template <typename TypeClass, typename... Args>
    inline void update_object_t(const int &id, Args &...args)
    {
        TypeClass object(id, std::forward<Args>(args)...);
        storage.update<TypeClass>(object);
    }

    bool validate_user(const std::string &username, const std::string &password_hash);

    bool perform_transaction(const int &user_id, const int &product_id, const int &amount);
};