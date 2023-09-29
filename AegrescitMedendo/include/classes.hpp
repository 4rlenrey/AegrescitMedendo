#pragma once

#include "utils.hpp"

class User;

class Product
{
public:
    int id;
    std::string title;
    std::string description;
    int amount;
    double price;
    int seller_id;
    int category_id;

    auto tie() const
    {
        return std::tie(id);
    }

    Product() = default;

    inline Product(const crow::json::rvalue &jsonValue, const int &object_id)
    {
        id = object_id;
        title = jsonValue["title"].s();
        description = jsonValue["description"].s();
        amount = jsonValue["amount"].i();
        price = jsonValue["price"].d();
        seller_id = jsonValue["seller_id"].i();
        category_id = jsonValue["category_id"].i();
    };

    Product(const Product &) = default;
    Product &operator=(const Product &) = default;

    crow::json::wvalue as_json() const
    {
        crow::json::wvalue product_json;

        product_json["id"] = id;
        product_json["title"] = title;
        product_json["description"] = description;
        product_json["amount"] = amount;
        product_json["price"] = price;
        product_json["seller_id"] = seller_id;
        product_json["category_id"] = category_id;

        return product_json;
    }
};

class Category
{
public:
    int id;
    std::string name;
    bool is_active;

    Category() = default;
    Category(const Category &) = default;
    Category &operator=(const Category &) = default;

    inline Category(const crow::json::rvalue &jsonValue, const int &object_id)
    {
        id = object_id;
        name = jsonValue["name"].s();
        is_active = jsonValue["is_active"].b();
    };

    auto tie() const
    {
        return std::tie(id);
    }

    crow::json::wvalue as_json() const
    {
        crow::json::wvalue category_json;

        category_json["id"] = id;
        category_json["name"] = name;
        category_json["is_active"] = is_active ? "True" : "False";

        return category_json;
    }
};

class Review
{
public:
    int id;
    int user_id;
    int product_id;
    int rating;
    std::string content;

    Review() = default;
    Review(const Review &) = default;
    Review &operator=(const Review &) = default;

    inline Review(const crow::json::rvalue &jsonValue, const int &object_id)
    {
        id = object_id;
        user_id = jsonValue["user_id"].i();
        product_id = jsonValue["product_id"].i();
        rating = jsonValue["rating"].i();
        content = jsonValue["content"].s();
    };

    auto tie() const
    {
        return std::tie(id);
    }

    crow::json::wvalue as_json() const
    {
        crow::json::wvalue product_bought_json;

        product_bought_json["id"] = id;
        product_bought_json["user_id"] = user_id;
        product_bought_json["product_id"] = product_id;
        product_bought_json["rating"] = rating;
        product_bought_json["content"] = content;

        return product_bought_json;
    }
};

class ProductBought
{
public:
    int id;
    int amount;
    double price;
    int buyer_id;
    int product_id;

    ProductBought() = default;
    ProductBought(const ProductBought &) = default;
    ProductBought(int _id, int _amount, double _price, int _buyer_id, int _product_id)
        : id(_id), amount(_amount), price(_price), buyer_id(_buyer_id), product_id(_product_id) {};
    ProductBought &operator=(const ProductBought &) = default;

    inline ProductBought(const crow::json::rvalue &jsonValue, const int &object_id)
    {
        id = object_id;
        amount = jsonValue["amount"].i();
        buyer_id = jsonValue["buyer_id"].i();
        product_id = jsonValue["product_id"].i();
        price = jsonValue["price"].d();
    };

    auto tie() const
    {
        return std::tie(id);
    }

    crow::json::wvalue as_json() const
    {
        crow::json::wvalue product_bought_json;

        product_bought_json["id"] = id;
        product_bought_json["buyer_id"] = buyer_id;
        product_bought_json["product_id"] = product_id;
        product_bought_json["amount"] = amount;
        product_bought_json["price"] = price;

        return product_bought_json;
    }
};

class User
{
public:
    unsigned long id;
    double balance;
    std::string name;
    std::string surname;
    std::string email;
    std::string phone;
    std::string username;
    std::string password; // hash

    User() = default;
    User(const User &) = default;
    User &operator=(const User &) = default;

    inline User(const crow::json::rvalue &jsonValue, const int &object_id)
    {
        id = object_id;
        balance = jsonValue["balance"].d();
        name = jsonValue["name"].s();
        surname = jsonValue["surname"].s();
        email = jsonValue["email"].s();
        phone = jsonValue["phone"].s();

        username = jsonValue["username"].s();
        password = utils::sha256(jsonValue["password"].s());
    };

    auto tie() const
    {
        return std::tie(id);
    }

    crow::json::wvalue as_json() const
    {
        crow::json::wvalue user_json;

        user_json["id"] = id;
        user_json["username"] = username;
        // user_json["password"] = password;
        user_json["balance"] = balance;
        user_json["name"] = name;
        user_json["surname"] = surname;
        user_json["email"] = email;
        user_json["phone"] = phone;

        return user_json;
    }
};
