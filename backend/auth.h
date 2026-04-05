#pragma once
#include "models.h"
#include <string>

class Auth {
public:
    static std::string generateToken();
    static User* authenticate(std::vector<User>& users, const std::string& token);
    static User* registerUser(std::vector<User>& users, const std::string& username, const std::string& password);
    static User* loginUser(std::vector<User>& users, const std::string& username, const std::string& password);
};
