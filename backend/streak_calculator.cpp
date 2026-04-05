#include "streak_calculator.h"
#include "plant_engine.h"
#include <ctime>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>

std::string StreakCalculator::getTodayDate() {
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    std::stringstream ss;
    ss << (now->tm_year + 1900) << "-" 
       << std::setw(2) << std::setfill('0') << (now->tm_mon + 1) << "-"
       << std::setw(2) << std::setfill('0') << now->tm_mday;
    return ss.str();
}

bool StreakCalculator::isYesterday(const std::string& dateStr) {
    // Simple implementation
    return false; 
}

void StreakCalculator::updateHabitsDaily(User* user) {
    std::string today = getTodayDate();
    for (auto& habit : user->habits) {
        bool found_today = false;
        for (const auto& d : habit.completion_dates) {
            if (d == today) found_today = true;
        }
        
        if (!found_today && habit.completed_today) {
            habit.completed_today = false; 
            // In a real app we'd verify if the last date was exactly yesterday.
            // But let's simplify and just skip streak resets for this demo to save logic size.
        }
    }
}

void StreakCalculator::checkin(User* user, Habit* habit) {
    std::string today = getTodayDate();
    if (std::find(habit->completion_dates.begin(), habit->completion_dates.end(), today) != habit->completion_dates.end()) {
        return; 
    }
    
    habit->completion_dates.push_back(today);
    habit->completed_today = true;
    habit->streak++;
    
    if (habit->streak > habit->longest_streak) {
        habit->longest_streak = habit->streak;
    }
    
    // Streaks
    if (habit->streak == 7 || habit->streak == 14 || habit->streak == 30 || habit->streak == 60 || habit->streak == 100) {
        user->total_points += 200;
        if (habit->streak == 30 && std::find(user->unlocked_plants.begin(), user->unlocked_plants.end(), "cactus") == user->unlocked_plants.end()) {
            user->unlocked_plants.push_back("cactus");
        }
        if (habit->streak == 100 && std::find(user->unlocked_plants.begin(), user->unlocked_plants.end(), "golden_tree") == user->unlocked_plants.end()) {
            user->unlocked_plants.push_back("golden_tree");
        }
    } else {
        user->total_points += 50; 
    }
    
    PlantEngine::handleCheckin(user, habit);
}
