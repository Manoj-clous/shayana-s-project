#pragma once
#include "models.h"
#include <string>

class StreakCalculator {
public:
    static std::string getTodayDate();
    static bool isYesterday(const std::string& dateStr);
    static void updateHabitsDaily(User* user);
    static void checkin(User* user, Habit* habit);
};
