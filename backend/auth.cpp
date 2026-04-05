#include "auth.h"
#include <random>

std::string Auth::generateToken() {
    std::string token = "";
    static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);
    for (int i = 0; i < 32; ++i) {
        token += alphanum[dis(gen)];
    }
    return token;
}

User* Auth::authenticate(std::vector<User>& users, const std::string& token) {
    if (token.empty()) return nullptr;
    for (auto& u : users) {
        if (u.token == token) return &u;
    }
    return nullptr;
}

User* Auth::registerUser(std::vector<User>& users, const std::string& username, const std::string& password) {
    for (auto& u : users) {
        if (u.username == username) return nullptr;
    }
    User newUser;
    newUser.id = users.empty() ? 1 : users.back().id + 1;
    newUser.username = username;
    // Simple mock hash
    newUser.password_hash = password + "_hashed"; 
    newUser.token = generateToken();
    newUser.total_points = 0;
    newUser.level = 1;
    newUser.unlocked_plants = {"rose"};
    users.push_back(newUser);
    return &users.back();
}

User* Auth::loginUser(std::vector<User>& users, const std::string& username, const std::string& password) {
    std::string expected_hash = password + "_hashed";
    for (auto& u : users) {
        if (u.username == username && u.password_hash == expected_hash) {
            u.token = generateToken(); // Reset token on login
            return &u;
        }
    }
    return nullptr;
}
