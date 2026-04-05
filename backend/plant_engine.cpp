#include "plant_engine.h"

void PlantEngine::updateLevel(User* user) {
    user->level = 1 + (user->total_points / 500);
    
    // Unlock logic
    if (user->level >= 2 && std::find(user->unlocked_plants.begin(), user->unlocked_plants.end(), "sunflower") == user->unlocked_plants.end()) {
        user->unlocked_plants.push_back("sunflower");
    }
    if (user->level >= 3 && std::find(user->unlocked_plants.begin(), user->unlocked_plants.end(), "oak") == user->unlocked_plants.end()) {
        user->unlocked_plants.push_back("oak");
    }
    if (user->level >= 5 && std::find(user->unlocked_plants.begin(), user->unlocked_plants.end(), "cherry_blossom") == user->unlocked_plants.end()) {
        user->unlocked_plants.push_back("cherry_blossom");
    }
}

void PlantEngine::handleCheckin(User* user, Habit* habit) {
    user->total_points += 10;
    habit->health = std::min(1.0f, habit->health + 0.15f); // Recovery
    
    // Growth logic - every 7th checkin increases stage, max out at 5
    if (habit->streak % 7 == 0 && habit->plant_stage < 5) {
        habit->plant_stage++;
    }
    
    updateLevel(user);
}

void PlantEngine::handleMissedDay(User* user, Habit* habit) {
    habit->health -= 0.15f;
    if (habit->health < 0.0f) habit->health = 0.0f;
}
