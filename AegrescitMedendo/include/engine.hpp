#pragma once
#include "middleware.hpp"
#include "storage.hpp"

class AegrescitMedendo
{
    void setup_routes();

protected:
    crow::App<Authorization> app;

public:
    std::shared_ptr<Storage> storage;
    AegrescitMedendo();
    void run();
};