#include "habit_manager.h"
#include <algorithm>

Habit HabitManager::createHabit(User* user, const std::string& name, const std::string& category, const std::string& plant_type) {
    Habit h;
    h.id = user->habits.empty() ? 1 : user->habits.back().id + 1;
    h.name = name;
    h.category = category;
    h.streak = 0;
    h.longest_streak = 0;
    h.plant_stage = 0;
    h.plant_type = plant_type;
    h.completed_today = false;
    h.health = 1.0f;
    user->habits.push_back(h);
    return h;
}

bool HabitManager::updateHabit(User* user, int habit_id, const std::string& name, const std::string& category, const std::string& plant_type) {
    for (auto& h : user->habits) {
        if (h.id == habit_id) {
            if (!name.empty()) h.name = name;
            if (!category.empty()) h.category = category;
            if (!plant_type.empty()) h.plant_type = plant_type;
            return true;
        }
    }
    return false;
}

bool HabitManager::deleteHabit(User* user, int habit_id) {
    auto it = std::remove_if(user->habits.begin(), user->habits.end(), [habit_id](const Habit& h) {
        return h.id == habit_id;
    });
    if (it != user->habits.end()) {
        user->habits.erase(it, user->habits.end());
        return true;
    }
    return false;
}

Habit* HabitManager::getHabit(User* user, int habit_id) {
    for (auto& h : user->habits) {
        if (h.id == habit_id) return &h;
    }
    return nullptr;
}
