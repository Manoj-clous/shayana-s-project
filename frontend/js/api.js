const DB_KEY = 'habit_garden_data';

class LocalDB {
    static load() {
        const data = localStorage.getItem(DB_KEY);
        if (data) return JSON.parse(data);
        return { users: [] };
    }
    static save(data) {
        localStorage.setItem(DB_KEY, JSON.stringify(data));
    }
}

class API {
    static get token() { return localStorage.getItem('token'); }
    static set token(val) { localStorage.setItem('token', val); }

    static getCurrentUser(db) {
        if (!this.token) throw new Error("Missing token");
        const user = db.users.find(u => u.token === this.token);
        if (!user) {
            this.token = '';
            throw new Error("Invalid token");
        }
        return user;
    }

    static async login(username, password) {
        const db = LocalDB.load();
        const user = db.users.find(u => u.username === username && u.password === password);
        if (user) {
            user.token = Math.random().toString(36).substring(2) + Date.now().toString(36);
            LocalDB.save(db);
            return { token: user.token, message: "Login successful" };
        }
        throw new Error("Invalid credentials");
    }

    static async register(username, password) {
        const db = LocalDB.load();
        if (db.users.find(u => u.username === username)) {
            throw new Error("Username taken");
        }
        const newUser = {
            id: db.users.length ? db.users[db.users.length-1].id + 1 : 1,
            username,
            password,
            token: Math.random().toString(36).substring(2) + Date.now().toString(36),
            total_points: 0,
            level: 1,
            unlocked_plants: ["rose"],
            habits: []
        };
        db.users.push(newUser);
        LocalDB.save(db);
        return { token: newUser.token, message: "Registered successfully" };
    }

    static async getHabits() {
        const db = LocalDB.load();
        const user = this.getCurrentUser(db);
        
        // Emulate daily streak checks
        const today = new Date().toISOString().split('T')[0];
        user.habits.forEach(h => {
            if (!h.completion_dates.includes(today) && h.completed_today) {
                h.completed_today = false;
            }
        });
        LocalDB.save(db);
        return user.habits;
    }

    static async addHabit(name, category, plant_type) {
        const db = LocalDB.load();
        const user = this.getCurrentUser(db);
        const habit = {
            id: user.habits.length ? user.habits[user.habits.length-1].id + 1 : 1,
            name,
            category,
            plant_type,
            streak: 0,
            longest_streak: 0,
            plant_stage: 0,
            completed_today: false,
            health: 1.0,
            completion_dates: []
        };
        user.habits.push(habit);
        LocalDB.save(db);
        return habit;
    }

    static async checkin(id) {
        const db = LocalDB.load();
        const user = this.getCurrentUser(db);
        const habit = user.habits.find(h => h.id === parseInt(id));
        if (!habit) throw new Error("Habit not found");

        const today = new Date().toISOString().split('T')[0];
        if (!habit.completion_dates.includes(today)) {
            habit.completion_dates.push(today);
            habit.completed_today = true;
            habit.streak++;
            if (habit.streak > habit.longest_streak) habit.longest_streak = habit.streak;
            
            // Emulate PlantEngine
            habit.plant_stage = Math.floor(habit.streak / 3);
            if (habit.plant_stage > 5) habit.plant_stage = 5;
            
            // Emulate streaks rewards
            if ([7, 14, 30, 60, 100].includes(habit.streak)) {
                user.total_points += 200;
                if (habit.streak === 30 && !user.unlocked_plants.includes('cactus')) user.unlocked_plants.push('cactus');
                if (habit.streak === 100 && !user.unlocked_plants.includes('golden_tree')) user.unlocked_plants.push('golden_tree');
            } else {
                user.total_points += 50;
            }
            LocalDB.save(db);
        }
        return habit;
    }

    static async deleteHabit(id) {
        const db = LocalDB.load();
        const user = this.getCurrentUser(db);
        user.habits = user.habits.filter(h => h.id !== parseInt(id));
        LocalDB.save(db);
        return { message: "Deleted" };
    }

    static async getStats() {
        const db = LocalDB.load();
        const user = this.getCurrentUser(db);
        return {
            total_points: user.total_points,
            level: user.level,
            unlocked_plants: user.unlocked_plants
        };
    }
}
