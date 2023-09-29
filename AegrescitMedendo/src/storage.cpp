#include "../include/storage.hpp"

bool Storage::validate_user(const std::string &username, const std::string &password_hash)
{
    using namespace sqlite_orm;
    auto userExists = this->storage.count<User>(
        where(c(&User::username) == username && c(&User::password) == password_hash));
    return (userExists > 0);
}

User Storage::get_user(const std::string &username)
{
    auto users = storage.get_all<User>(sqlite_orm::where(sqlite_orm::c(&User::username) == username));
    if (!users.empty())
    {
        return users[0];
    }
    else
    {
        throw std::runtime_error("User not found!");
    }
}

bool Storage::perform_transaction(const int &user_id,
                                  const int &product_id,
                                  const int &amount)
{
    auto user{get_object<User>(user_id)};
    auto product{get_object<Product>(product_id)};
    auto seller{get_object<User>(product.seller_id)};

    if (user.id == seller.id)
    {
        return false;
    }
    if (user.balance < product.price * amount || product.amount < amount)
    {
        return false;
    }

    ProductBought order{1, amount, product.price * amount, user_id, product.id};
    storage.insert<ProductBought>(order);

    user.balance -= product.price * amount;
    product.amount -= amount;
    seller.balance += product.price * amount;

    update_object<User>(user);
    update_object<Product>(product);
    update_object<User>(seller);

    return true;
}