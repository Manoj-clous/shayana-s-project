#pragma once
#include "models.h"
#include <string>
#include <vector>

class FileStorage {
public:
    static std::vector<User> loadUsers();
    static void saveUsers(const std::vector<User>& users);
private:
    static const std::string FILE_PATH;
};
