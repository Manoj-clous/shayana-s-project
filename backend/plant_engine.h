#pragma once
#include "models.h"

class PlantEngine {
public:
    static void handleCheckin(User* user, Habit* habit);
    static void handleMissedDay(User* user, Habit* habit);
    static void updateLevel(User* user);
};
