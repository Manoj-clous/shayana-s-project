#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#endif
#include "include/httplib.h"
#include "include/json.hpp"
#include "models.h"
#include "file_storage.h"
#include "auth.h"
#include "habit_manager.h"
#include "streak_calculator.h"
#include <iostream>

using nlohmann::json;

#define ENABLE_CORS(res) \
    res.set_header("Access-Control-Allow-Origin", "*"); \
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"); \
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization")

int main() {
    httplib::Server svr;
    
    std::vector<User> users = FileStorage::loadUsers();
    
    auto save = [&]() {
        FileStorage::saveUsers(users);
    };

    // Serve Static Files
    if (!svr.set_mount_point("/", "./frontend")) {
        svr.set_mount_point("/", "../frontend");
    }

    svr.Options(R"(.*)", [](const httplib::Request&, httplib::Response& res) {
        ENABLE_CORS(res);
        res.status = 200;
    });

    svr.Post("/api/auth/register", [&](const httplib::Request& req, httplib::Response& res) {
        ENABLE_CORS(res);
        try {
            auto j = json::parse(req.body);
            std::string username = j.at("username");
            std::string password = j.at("password");
            
            User* user = Auth::registerUser(users, username, password);
            if (user) {
                save();
                res.status = 200;
                res.set_content(json{{"token", user->token}, {"message", "Registered successfully"}}.dump(), "application/json");
            } else {
                res.status = 400;
                res.set_content(json{{"error", "Username taken"}}.dump(), "application/json");
            }
        } catch(...) {
            res.status = 400;
            res.set_content(json{{"error", "Invalid body"}}.dump(), "application/json");
        }
    });

    svr.Post("/api/auth/login", [&](const httplib::Request& req, httplib::Response& res) {
        ENABLE_CORS(res);
        try {
            auto j = json::parse(req.body);
            std::string username = j.at("username");
            std::string password = j.at("password");
            
            User* user = Auth::loginUser(users, username, password);
            if (user) {
                save();
                res.status = 200;
                res.set_content(json{{"token", user->token}, {"message", "Login successful"}}.dump(), "application/json");
            } else {
                res.status = 401;
                res.set_content(json{{"error", "Invalid credentials"}}.dump(), "application/json");
            }
        } catch(...) {
            res.status = 400;
            res.set_content(json{{"error", "Invalid body"}}.dump(), "application/json");
        }
    });

    auto authenticate = [&](const httplib::Request& req, httplib::Response& res, User*& user) -> bool {
        if (!req.has_header("Authorization")) {
            res.status = 401;
            res.set_content(json{{"error", "Missing token"}}.dump(), "application/json");
            return false;
        }
        std::string token = req.get_header_value("Authorization");
        if (token.find("Bearer ") == 0) {
            token = token.substr(7);
        }
        user = Auth::authenticate(users, token);
        if (!user) {
            res.status = 401;
            res.set_content(json{{"error", "Invalid token"}}.dump(), "application/json");
            return false;
        }
        StreakCalculator::updateHabitsDaily(user);
        return true;
    };

    svr.Get("/api/habits", [&](const httplib::Request& req, httplib::Response& res) {
        ENABLE_CORS(res);
        User* user = nullptr;
        if (!authenticate(req, res, user)) return;
        save();
        
        json j = user->habits;
        res.set_content(j.dump(), "application/json");
    });

    svr.Post("/api/habits", [&](const httplib::Request& req, httplib::Response& res) {
        ENABLE_CORS(res);
        User* user = nullptr;
        if (!authenticate(req, res, user)) return;
        
        try {
            auto j = json::parse(req.body);
            Habit h = HabitManager::createHabit(user, j.at("name"), j.at("category"), j.at("plant_type"));
            save();
            json resp = h;
            res.set_content(resp.dump(), "application/json");
        } catch(...) {
            res.status = 400;
            res.set_content(json{{"error", "Invalid body"}}.dump(), "application/json");
        }
    });

    svr.Put(R"(/api/habits/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
        ENABLE_CORS(res);
        User* user = nullptr;
        if (!authenticate(req, res, user)) return;
        
        int id = std::stoi(req.matches[1]);
        try {
            auto j = json::parse(req.body);
            std::string name = j.value("name", "");
            std::string cat = j.value("category", "");
            std::string plant = j.value("plant_type", "");
            
            if (HabitManager::updateHabit(user, id, name, cat, plant)) {
                save();
                res.status = 200;
                res.set_content(json{{"message", "Habit updated"}}.dump(), "application/json");
            } else {
                res.status = 404;
                res.set_content(json{{"error", "Habit not found"}}.dump(), "application/json");
            }
        } catch(...) {
            res.status = 400;
            res.set_content(json{{"error", "Invalid body"}}.dump(), "application/json");
        }
    });

    svr.Delete(R"(/api/habits/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
        ENABLE_CORS(res);
        User* user = nullptr;
        if (!authenticate(req, res, user)) return;
        
        int id = std::stoi(req.matches[1]);
        if (HabitManager::deleteHabit(user, id)) {
            save();
             res.status = 200;
             res.set_content(json{{"message", "Habit deleted"}}.dump(), "application/json");
        } else {
            res.status = 404;
            res.set_content(json{{"error", "Habit not found"}}.dump(), "application/json");
        }
    });

    svr.Post(R"(/api/habits/(\d+)/checkin)", [&](const httplib::Request& req, httplib::Response& res) {
        ENABLE_CORS(res);
        User* user = nullptr;
        if (!authenticate(req, res, user)) return;
        
        int id = std::stoi(req.matches[1]);
        Habit* h = HabitManager::getHabit(user, id);
        if (h) {
            StreakCalculator::checkin(user, h);
            save();
            res.status = 200;
            json resp = *h;
            res.set_content(resp.dump(), "application/json");
        } else {
            res.status = 404;
            res.set_content(json{{"error", "Habit not found"}}.dump(), "application/json");
        }
    });

    svr.Get("/api/garden", [&](const httplib::Request& req, httplib::Response& res) {
        ENABLE_CORS(res);
        User* user = nullptr;
        if (!authenticate(req, res, user)) return;
        
        json j = user->habits; 
        res.set_content(j.dump(), "application/json");
    });
    
    svr.Get("/api/stats", [&](const httplib::Request& req, httplib::Response& res) {
        ENABLE_CORS(res);
        User* user = nullptr;
        if (!authenticate(req, res, user)) return;
        
        json j;
        j["total_points"] = user->total_points;
        j["level"] = user->level;
        j["unlocked_plants"] = user->unlocked_plants;
        res.set_content(j.dump(), "application/json");
    });

    std::cout << "Starting Habit Garden Server on port 8080..." << std::endl;
    svr.listen("0.0.0.0", 8080);
    return 0;
}
