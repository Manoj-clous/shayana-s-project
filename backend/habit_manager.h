#pragma once
#include "models.h"
#include <string>

class HabitManager {
public:
    static Habit createHabit(User* user, const std::string& name, const std::string& category, const std::string& plant_type);
    static bool updateHabit(User* user, int habit_id, const std::string& name, const std::string& category, const std::string& plant_type);
    static bool deleteHabit(User* user, int habit_id);
    static Habit* getHabit(User* user, int habit_id);
};
