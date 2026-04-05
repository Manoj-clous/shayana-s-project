#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include "json.hpp"

using nlohmann::json;

struct Habit {
    int id;
    std::string name;
    std::string category;
    int streak;
    int longest_streak;
    std::vector<std::string> completion_dates;
    int plant_stage;
    std::string plant_type;
    bool completed_today;
    float health;
};

struct User {
    int id;
    std::string username;
    std::string password_hash;
    std::string token;
    std::vector<Habit> habits;
    int total_points;
    int level;
    std::vector<std::string> unlocked_plants;
};

// nlohmann::json serialization macros
inline void to_json(json& j, const Habit& h) {
    j = json{{"id", h.id}, {"name", h.name}, {"category", h.category},
             {"streak", h.streak}, {"longest_streak", h.longest_streak},
             {"completion_dates", h.completion_dates}, {"plant_stage", h.plant_stage},
             {"plant_type", h.plant_type}, {"completed_today", h.completed_today},
             {"health", h.health}};
}

inline void from_json(const json& j, Habit& h) {
    j.at("id").get_to(h.id);
    j.at("name").get_to(h.name);
    j.at("category").get_to(h.category);
    j.at("streak").get_to(h.streak);
    j.at("longest_streak").get_to(h.longest_streak);
    j.at("completion_dates").get_to(h.completion_dates);
    j.at("plant_stage").get_to(h.plant_stage);
    j.at("plant_type").get_to(h.plant_type);
    j.at("completed_today").get_to(h.completed_today);
    j.at("health").get_to(h.health);
}

inline void to_json(json& j, const User& u) {
    j = json{{"id", u.id}, {"username", u.username}, {"password_hash", u.password_hash},
             {"token", u.token}, {"habits", u.habits}, {"total_points", u.total_points},
             {"level", u.level}, {"unlocked_plants", u.unlocked_plants}};
}

inline void from_json(const json& j, User& u) {
    j.at("id").get_to(u.id);
    j.at("username").get_to(u.username);
    j.at("password_hash").get_to(u.password_hash);
    j.at("token").get_to(u.token);
    j.at("habits").get_to(u.habits);
    j.at("total_points").get_to(u.total_points);
    j.at("level").get_to(u.level);
    j.at("unlocked_plants").get_to(u.unlocked_plants);
}
