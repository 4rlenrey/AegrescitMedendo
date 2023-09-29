#include "../include/engine.hpp"

AegrescitMedendo::AegrescitMedendo()
{
    storage = std::make_shared<Storage>();
    setup_routes();
}

void AegrescitMedendo::run()
{
    std::cout << "Started AegrescitMedendo! Good luck on maintaining this creature\n";
    // app.loglevel(crow::LogLevel::Warning);
    app.port(80).multithreaded().run();
}

void AegrescitMedendo::setup_routes()
{

    // Catchall route to 404 page
    CROW_CATCHALL_ROUTE(app)
    ([this](const crow::request &req) -> crow::response
     { return Pages::p404(); });

    CROW_ROUTE(app, "/")
    ([&](const crow::request &req)
     {  crow::response res;
        res.redirect("/index");
        return res; });

    CROW_ROUTE(app, "/login")
    ([]() -> crow::response
     { return Pages::login(); });

    CROW_ROUTE(app, "/about")
    ([]() -> crow::response
     { return Pages::about(); });

    CROW_ROUTE(app, "/refresh")
    ([](const crow::request &req) -> crow::response
     { return Pages::refresh(req); });

    CROW_ROUTE(app, "/api/buy")
        .methods(crow::HTTPMethod::POST)
        .CROW_MIDDLEWARES(app, Authorization)([this](const crow::request &req) -> crow::response
                                              {auto& ctx = app.get_context<Authorization>(req);
     return Rhandlers::perform_transaction(req, this->storage, ctx.username); });

    CROW_ROUTE(app, "/auth/refresh")
        .methods(crow::HTTPMethod::POST)([this](const crow::request &req) -> crow::response
                                         { return Rhandlers::refresh_token(req, this->storage); });

    CROW_ROUTE(app, "/auth/obtain")
        .methods(crow::HTTPMethod::POST)([this](const crow::request &req) -> crow::response
                                         { return Rhandlers::obtain_token(req, this->storage); });

    CROW_ROUTE(app, "/profile/<int>").CROW_MIDDLEWARES(app, Authorization)([&](const crow::request &req, int id) -> crow::response
                                                                           {auto& ctx = app.get_context<Authorization>(req);
    return Pages::user(ctx.username, id, storage); });

    ////////////////////////// OTHER/STATIC ///////////////////////

    CROW_ROUTE(app, "/common.css")
    ([]() -> crow::response
     { return crow::mustache::load_text("common.css"); });

    // TODO: THIS IS MOST LIKELY UNSAFE. FIX IT
    CROW_ROUTE(app, "/static/<string>")
    ([](std::string f) -> crow::response
     {
        crow::response resp;
        resp.set_static_file_info("static/" + f);
        return resp; });

    /////////////////////////////// PROTECTED ROUTES //////////////////////////
    CROW_ROUTE(app, "/index").CROW_MIDDLEWARES(app, Authorization)([&](const crow::request &req) -> crow::response
                                                                   {auto& ctx = app.get_context<Authorization>(req);
    return Pages::main(ctx.username, storage); });

    CROW_ROUTE(app, "/admin").CROW_MIDDLEWARES(app, Authorization)([&](const crow::request &req) -> crow::response
                                                                   {
    auto& ctx = app.get_context<Authorization>(req);
      return Pages::admin(storage); });

    //////////////////////////// Dynamic API CRUD routes //////////////////////////////////

    CROW_ROUTE(app, "/api/user")
        .methods(crow::HTTPMethod::POST)([this](const crow::request &req) -> crow::response
                                         { return CRUDns::create_object_handler<User>(req, this->storage); });

    CROW_ROUTE(app, "/product/<int>")
        .CROW_MIDDLEWARES(app, Authorization)([&](const crow::request &req, int id) -> crow::response
                                              {
        auto& ctx = app.get_context<Authorization>(req);
        return Pages::product(ctx.username, storage, id); });

    CROW_ROUTE(app, "/api/<string>")
        .methods(crow::HTTPMethod::GET)
        .CROW_MIDDLEWARES(app, Authorization)([&](const crow::request &req, const std::string &route) -> crow::response
                                              {
            
        auto& ctx = app.get_context<Authorization>(req);
            if (route == "product") {
                return CRUDns::get_objects<Product>(storage);
            }
            // else if (route == "user") {
            //     return CRUDns::get_objects<User>(storage);
            // }
             else if (route == "review") {
                return CRUDns::get_objects<Review>(storage);
            } else if (route == "category") {
                return CRUDns::get_objects<Category>(storage);
            } else {
                return crow::response(404);
            } });

    CROW_ROUTE(app, "/api/<string>")
        .methods(crow::HTTPMethod::POST)
        .CROW_MIDDLEWARES(app, Authorization)([&](const crow::request &req, const std::string &route) -> crow::response
                                              {
        auto& ctx = app.get_context<Authorization>(req);
        if (route == "product") {
            return CRUDns::create_product(req, storage, ctx.username);
        } else if (route == "user") {
            return CRUDns::create_object_handler<User>(req, storage);
        } else if (route == "review") {
            return CRUDns::create_object_handler<Review>(req, storage);
        } else if (route == "category") {
            return CRUDns::create_object_handler<Category>(req, storage);
        } else {
            return crow::response(404);
        } });

    CROW_ROUTE(app, "/api/<string>/<int>")
        .methods(crow::HTTPMethod::DELETE)
        .CROW_MIDDLEWARES(app, Authorization)([&](const crow::request &req, const std::string &route, int id) -> crow::response
                                              {
        auto& ctx = app.get_context<Authorization>(req);
        if (route == "product") {
            return CRUDns::delete_object<Product>(id, storage);
        } else if (route == "user") {
            return CRUDns::delete_object<User>(id, storage);
        } else if (route == "review") {
            return CRUDns::delete_object<Review>(id, storage);
        } else if (route == "category") {
            return CRUDns::delete_object<Category>(id, storage);
        } else {
            return crow::response(404);
        } });

    CROW_ROUTE(app, "/api/<string>/<int>")
        .methods(crow::HTTPMethod::PATCH)
        .CROW_MIDDLEWARES(app, Authorization)([&](const crow::request &req, const std::string &route, int id) -> crow::response
                                              {
        auto& ctx = app.get_context<Authorization>(req);
        if (route == "product") {
            return CRUDns::update_object_handler<Product>(req, this->storage, id);
        } else if (route == "user") {
            return CRUDns::update_object_handler<User>(req, this->storage, id);
        } else if (route == "review") {
            return CRUDns::update_object_handler<Review>(req, this->storage, id);
        } else if (route == "category") {
            return CRUDns::update_object_handler<Category>(req, this->storage, id);
        } else {
            return crow::response(404);
        } });
}
