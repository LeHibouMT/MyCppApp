#ifndef USERS_H
#define USERS_H

#include <crow.h>
#include <sqlite3.h>
#include <string>

struct User {
    int id;
    std::string name;
    std::string email;
};

class UserDatabase {
public:
    UserDatabase(const std::string& db_path);
    ~UserDatabase();

    void createTable();
    crow::json::wvalue getAllUsers();
    crow::json::wvalue getUserById(int id);
    bool addUser(const std::string& name, const std::string& email);
    bool updateUser(int id, const std::string& name, const std::string& email);
    bool deleteUser(int id);

private:
    sqlite3* db;
};

void setup_user_routes(crow::SimpleApp& app, UserDatabase& userDB);

#endif // USERS_H
