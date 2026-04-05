#include "file_storage.h"
#include <fstream>
#include <iostream>

const std::string FileStorage::FILE_PATH = "../data/users.json";

std::vector<User> FileStorage::loadUsers() {
    std::ifstream file(FILE_PATH);
    if (!file.is_open()) {
        return {};
    }
    json j;
    try {
        file >> j;
        return j.get<std::vector<User>>();
    } catch (...) {
        return {};
    }
}

void FileStorage::saveUsers(const std::vector<User>& users) {
    std::ofstream file(FILE_PATH);
    json j = users;
    file << j.dump(4);
}
