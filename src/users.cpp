#include "users.h"
#include <iostream>

UserDatabase::UserDatabase(const std::string& db_path) : db(nullptr) {
    if (sqlite3_open(db_path.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
    }
}

UserDatabase::~UserDatabase() {
    if (db) {
        sqlite3_close(db);
    }
}

void UserDatabase::createTable() {
    const char* sql = "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "email TEXT NOT NULL UNIQUE);";

    char* errorMessage;
    if (sqlite3_exec(db, sql, 0, 0, &errorMessage) != SQLITE_OK) {
        std::cerr << "Error creating table: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
    }
}

crow::json::wvalue UserDatabase::getAllUsers() {
    crow::json::wvalue result;
    sqlite3_stmt* stmt;
    const char* sql = "SELECT * FROM users;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

            crow::json::wvalue user;
            user["id"] = id;
            user["name"] = name;
            user["email"] = email;

            result["users"][std::to_string(id)] = std::move(user);
        }
        sqlite3_finalize(stmt);
    }
    return result;
}

crow::json::wvalue UserDatabase::getUserById(int id) {
    crow::json::wvalue result;
    sqlite3_stmt* stmt;
    const char* sql = "SELECT * FROM users WHERE id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            result["id"] = sqlite3_column_int(stmt, 0);
            result["name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            result["email"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        }
        sqlite3_finalize(stmt);
    }
    return result;
}

bool UserDatabase::addUser(const std::string& name, const std::string& email) {
    const char* sql = "INSERT INTO users (name, email) VALUES (?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            sqlite3_finalize(stmt);
            return true;
        }
        sqlite3_finalize(stmt);
    }
    return false;
}

bool UserDatabase::updateUser(int id, const std::string& name, const std::string& email) {
    const char* sql = "UPDATE users SET name = ?, email = ? WHERE id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, id);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            sqlite3_finalize(stmt);
            return true;
        }
        sqlite3_finalize(stmt);
    }
    return false;
}

bool UserDatabase::deleteUser(int id) {
    const char* sql = "DELETE FROM users WHERE id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            sqlite3_finalize(stmt);
            return true;
        }
        sqlite3_finalize(stmt);
    }
    return false;
}

void setup_user_routes(crow::SimpleApp& app, UserDatabase& userDB) {
    CROW_ROUTE(app, "/users").methods(crow::HTTPMethod::Get)
        ([&userDB]() {
        return crow::response(userDB.getAllUsers());
            });

    CROW_ROUTE(app, "/users/<int>").methods(crow::HTTPMethod::Get)
        ([&userDB](int id) {
        return crow::response(userDB.getUserById(id));
            });

    CROW_ROUTE(app, "/users").methods(crow::HTTPMethod::Post)
        ([&userDB](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || !body.has("name") || !body.has("email")) {
            return crow::response(400, "Invalid JSON format");
        }

        bool success = userDB.addUser(body["name"].s(), body["email"].s());
        return success ? crow::response(201, "User created") : crow::response(500, "Error adding user");
            });

    CROW_ROUTE(app, "/users/<int>").methods(crow::HTTPMethod::Put)
        ([&userDB](const crow::request& req, int id) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON format");

        bool success = userDB.updateUser(id, body["name"].s(), body["email"].s());
        return success ? crow::response(200, "User updated") : crow::response(404, "User not found");
            });

    CROW_ROUTE(app, "/users/<int>").methods(crow::HTTPMethod::Delete)
        ([&userDB](int id) {
        return userDB.deleteUser(id) ? crow::response(200, "User deleted") : crow::response(404, "User not found");
            });
}
