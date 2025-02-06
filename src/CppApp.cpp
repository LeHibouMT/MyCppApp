#include <crow.h>
#include "users.h"
#include "users.cpp"

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([]() {
        return "Hello, Crow!";
        });

    UserDatabase userDB("users.db");
    userDB.createTable();

    setup_user_routes(app,userDB);

    app.port(8080).multithreaded().run();
    return 0;
}