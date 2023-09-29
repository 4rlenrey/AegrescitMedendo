#include "../include/handler.hpp"
#include "../include/utils.hpp"
#include "../include/classes.hpp"

resp Rhandlers::refresh_token(const crow::request &req, std::shared_ptr<Storage> &storage)
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
        crow::json::wvalue response;
        auto username{jsonValue["username"].s()};
        auto refresh_token{jsonValue["refresh_token"].s()};

        auto info{utils::verify_token(refresh_token, username)};
        if (info.correct)
        {
            res.code = 200;
            response["access_token"] = utils::generate_token(username, utils::ACCESS);
            res.body = response.dump();
        }
    }
    catch (const std::exception &e)
    {
        res.code = 400;
        res.body = "Couldn't create that object: " + std::string(e.what());
        return res;
    }
    return res;
}

resp Rhandlers::obtain_token(const crow::request &req, std::shared_ptr<Storage> &storage)
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
        auto username{jsonValue["username"].s()};
        auto password{utils::sha256(jsonValue["password"].s())};

        crow::json::wvalue response;

        if (storage->validate_user(username, password))
        {
            res.code = 200;
            response["access_token"] = utils::generate_token(username, utils::ACCESS);
            response["refresh_token"] = utils::generate_token(username, utils::REFRESH);
            res.body = response.dump();
        }
    }
    catch (const std::exception &e)
    {
        res.code = 400;
        res.body = "Couldn't create that object: " + std::string(e.what());
        return res;
    }
    return res;
}

page Pages::user(const std::string &username, const int &id, std::shared_ptr<Storage> &storage)
{
    try // seller_id
    {
        crow::mustache::context ctx;
        auto categories{storage->get_jsons<Category>()};
        auto user = storage->get_user(username);
        if (user.id != id)
        {
            return crow::mustache::load("404.html").render(); // Maybe some code?
        }
        auto products{storage->get_product_jsons(id)}; // Should be the ones based on user id
        ctx["orders"] = std::move(storage->get_orders(user.id));

        ctx["categories"] = std::move(categories);
        ctx["products"] = std::move(products);
        ctx["username"] = std::move(username);
        ctx["name"] = std::move(user.name);
        ctx["surname"] = std::move(user.surname);
        ctx["email"] = std::move(user.email);
        ctx["phone"] = std::move(user.phone);
        ctx["balance"] = std::move(user.balance);
        ctx["id"] = std::move(user.id);
        return crow::mustache::load("user.mustache").render(ctx);
    }
    catch (const std::exception &exc)
    {
        std::cerr << exc.what();
    }
}

page Pages::admin(std::shared_ptr<Storage> &storage)
{
    crow::mustache::context ctx;

    auto products{storage->get_jsons<Product>()};
    auto categories{storage->get_jsons<Category>()};
    auto users{storage->get_jsons<User>()};
    auto reviews{storage->get_jsons<Review>()};

    ctx["products"] = std::move(products);
    ctx["category"] = std::move(categories);
    categories = storage->get_jsons<Category>();
    ctx["categories"] = std::move(categories);
    ctx["users"] = std::move(users);
    ctx["reviews"] = std::move(reviews);

    return crow::mustache::load("admin.mustache").render(ctx);
}

page Pages::p404()
{
    return crow::mustache::load("404.html").render();
}

page Pages::main(const std::string &username, std::shared_ptr<Storage> &storage)
{
    try
    {
        crow::mustache::context ctx;
        auto products{storage->get_jsons<Product>()};
        auto categories{storage->get_jsons<Category>()};
        auto user = storage->get_user(username);
        ctx["categories"] = std::move(categories);
        ctx["products"] = std::move(products);
        ctx["username"] = std::move(username);
        ctx["balance"] = std::move(user.balance);
        ctx["user_id"] = std::move(user.id);
        return crow::mustache::load("index.mustache").render(ctx);
    }
    catch (const std::exception &exc)
    {
        std::cerr << exc.what();
    }
}

page Pages::about()
{
    return crow::mustache::load("about.mustache").render();
}

page Pages::refresh(const crow::request &req)
{
    crow::mustache::context ctx;
    std::string originalPath = req.url_params.get("url");
    originalPath = utils::decodeURIComponent(originalPath);
    ctx["path"] = originalPath;

    return crow::mustache::load("refresh.mustache").render(ctx);
}

page Pages::login()
{
    crow::mustache::context ctx;
    return crow::mustache::load("login.mustache").render(ctx);
}

crow::json::wvalue mergeJson(crow::json::wvalue &&json1, crow::json::wvalue &&json2)
{
    crow::json::wvalue mergedJson;

    // Retrieve keys from json1
    std::vector<std::string> keys1 = std::move(json1).keys();
    for (const auto &key : keys1)
    {
        mergedJson[key] = std::move(json1[key]);
    }

    // Retrieve keys from json2
    std::vector<std::string> keys2 = std::move(json2).keys();
    for (const auto &key : keys2)
    {
        mergedJson[key] = std::move(json2[key]);
    }

    return mergedJson;
}

page Pages::product(const std::string &username, std::shared_ptr<Storage> &storage, const int &id)
{
    try // seller_id
    {
        crow::mustache::context ctx;
        auto categories{storage->get_jsons<Category>()};
        auto user = storage->get_user(username);
        auto product_o{storage->get_object<Product>(id)};

        ctx["title"] = std::move(product_o.title);
        ctx["description"] = std::move(product_o.description);
        ctx["amount"] = std::move(product_o.amount);
        ctx["price"] = std::move(product_o.price);
        ctx["category_id"] = std::move(product_o.category_id);

        ctx["categories"] = std::move(categories);
        ctx["username"] = std::move(username);
        ctx["balance"] = std::move(user.balance);
        ctx["user_id"] = std::move(user.id);
        ctx["product_id"] = std::move(id);

        if (user.id != product_o.seller_id)
        {
            auto reviews{storage->get_review_jsons(id)};
            ctx["reviews"] = std::move(reviews);
            return crow::mustache::load("buy_product.mustache").render(ctx);
        }

        return crow::mustache::load("product.mustache").render(ctx);
    }
    catch (const std::exception &exc)
    {
        std::cerr << exc.what();
        return crow::mustache::load("404.html").render();
    }
}

resp Rhandlers::perform_transaction(const crow::request &req, std::shared_ptr<Storage> &storage, std::string username)
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
        auto user_id{jsonValue["user_id"].i()};
        auto product_id{jsonValue["product_id"].i()};
        auto amount{jsonValue["amount"].i()};
        
        auto user = storage->get_user(username);

        if(user.id != user_id){
            res.code = 400;
            res.body = "You're pretending to be someone you're not";
            return res;
        }


        crow::json::wvalue response;
        if (storage->perform_transaction(user_id, product_id, amount))
        {
            res.code = 200;
        }
        else
        {
            res.code = 400;
        }
    }
    catch (const std::exception &e)
    {
        res.code = 400;
        res.body = "Couldn't create that object: " + std::string(e.what());
        return res;
    }
    return res;
}