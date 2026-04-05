// Plant visual definitions
const PLANT_STAGES = [
    '🌱', // 0 seed (actually sprout symbol but ok)
    '🌿', // 1 sprout
    '🪴', // 2 sapling
    '🪴', // 3 plant -> just using general pot
    '🌸', // 4 bloom -> generic bloom
    '🌳'  // 5 tree
];

const PLANT_TYPES = {
    'rose': { emoji: '🌹', name: 'Rose' },
    'lily': { emoji: '🪷', name: 'Lily' },
    'tulip': { emoji: '🌷', name: 'Tulip' },
    'fern': { emoji: '🌿', name: 'Fern' },
    'mushroom': { emoji: '🍄', name: 'Mushroom' },
    'bamboo': { emoji: '🎋', name: 'Bamboo' },
    'sunflower': { emoji: '🌻', name: 'Sunflower' },
    'oak': { emoji: '🌳', name: 'Oak Tree' },
    'cherry_blossom': { emoji: '🌸', name: 'Cherry Blossom' },
    'cactus': { emoji: '🌵', name: 'Cactus' },
    'golden_tree': { emoji: '🏵️', name: 'Golden Tree' } // Mocking with an emoji
};

let currentHabits = [];
let userStats = {};

// --- Initialization ---
document.addEventListener('DOMContentLoaded', () => {
    initAvatar();
    if (API.token) {
        showView('main-view');
        loadDashboard();
    } else {
        showView('auth-view');
    }
});

// --- Auth ---
let authMode = 'login';
function toggleAuth(mode) {
    authMode = mode;
    document.getElementById('tab-login').classList.toggle('active', mode === 'login');
    document.getElementById('tab-register').classList.toggle('active', mode === 'register');
    document.getElementById('auth-submit').innerText = mode === 'login' ? 'Login' : 'Register';
    document.getElementById('auth-error').innerText = '';
}

async function handleAuth(e) {
    e.preventDefault();
    const u = document.getElementById('username').value;
    const p = document.getElementById('password').value;
    const errEl = document.getElementById('auth-error');
    try {
        const res = authMode === 'login' ? await API.login(u, p) : await API.register(u, p);
        API.token = res.token;
        document.getElementById('password').value = '';
        showView('main-view');
        loadDashboard();
    } catch(err) {
        errEl.innerText = err.message;
    }
}

function logout() {
    API.token = '';
    showView('auth-view');
}

// --- Navigation ---
function showView(viewId) {
    document.querySelectorAll('.view').forEach(v => v.classList.remove('active'));
    document.getElementById(viewId).classList.add('active');
}

function navigate(pageId) {
    document.querySelectorAll('.page').forEach(p => p.classList.remove('active'));
    document.querySelectorAll('.btn-nav').forEach(b => b.classList.remove('active'));
    
    document.getElementById(`page-${pageId}`).classList.add('active');
    document.getElementById(`nav-${pageId}`).classList.add('active');

    if (pageId === 'dashboard') loadDashboard();
    if (pageId === 'manager') loadManager();
    if (pageId === 'stats') loadStats();
}

// --- Dashboard ---
async function loadDashboard() {
    try {
        currentHabits = await API.getHabits();
        userStats = await API.getStats();
        
        renderHabits();
        renderGarden();
        renderStats();
    } catch (err) {
        if (err.message === 'Invalid token' || err.message === 'Missing token') logout();
    }
}

function renderHabits() {
    const list = document.getElementById('habits-list');
    list.innerHTML = '';
    currentHabits.forEach(h => {
        const div = document.createElement('div');
        div.className = 'habit-card';
        div.innerHTML = `
            <div class="habit-info">
                <span class="habit-name">${h.name}</span>
                <span class="habit-streak">${h.category} | 🔥 ${h.streak} days</span>
            </div>
            <button class="check-btn ${h.completed_today ? 'completed' : ''}" onclick="checkin(${h.id}, event)">✓</button>
        `;
        list.appendChild(div);
    });
}

function renderGarden() {
    const grid = document.getElementById('garden-grid');
    grid.innerHTML = '';
    
    for (let i = 0; i < 12; i++) {
        const slot = document.createElement('div');
        slot.className = 'plant-slot';
        
        if (i < currentHabits.length) {
            const h = currentHabits[i];
            let emoji = PLANT_STAGES[Math.min(h.plant_stage, PLANT_STAGES.length - 1)];
            if (h.plant_stage >= 4 && PLANT_TYPES[h.plant_type]) {
                emoji = PLANT_TYPES[h.plant_type].emoji;
            }
            
            const p = document.createElement('div');
            p.className = `plant-emoji ${h.health < 0.3 ? 'wilted' : ''}`;
            p.innerText = emoji;
            p.style.animationDelay = `${Math.random() * 2}s`;
            
            const tooltip = document.createElement('div');
            tooltip.className = 'plant-tooltip';
            tooltip.innerHTML = `<b>${h.name}</b><br>Stage: ${h.plant_stage}<br>Health: ${Math.round(h.health * 100)}%`;
            
            slot.appendChild(p);
            slot.appendChild(tooltip);
        }
        grid.appendChild(slot);
    }
}

function renderStats() {
    document.getElementById('user-level').innerText = userStats.level;
    document.getElementById('user-points').innerText = userStats.total_points;
    
    const unlocked = document.getElementById('unlocked-plants');
    unlocked.innerHTML = '';
    (userStats.unlocked_plants || []).forEach(pt => {
        if(PLANT_TYPES[pt]) {
            const span = document.createElement('span');
            span.className = 'unlocked-item';
            span.innerText = PLANT_TYPES[pt].emoji;
            span.title = PLANT_TYPES[pt].name;
            unlocked.appendChild(span);
        }
    });
}

// --- Interactions ---
async function checkin(id, event) {
    // Fire particles instantly
    createParticles(event.clientX, event.clientY);
    say("Great job! Keep growing! ✨");
    try {
        await API.checkin(id);
        loadDashboard(); // reload to update state
    } catch(e) {
        console.error(e);
    }
}

// --- Particles Engine ---
const canvas = document.getElementById('particle-canvas');
const ctx = canvas.getContext('2d');
let particlesArray = [];

function resize() {
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
}
window.addEventListener('resize', resize);
resize();

class Particle {
    constructor(x, y) {
        this.x = x; this.y = y;
        this.size = Math.random() * 5 + 2;
        this.speedX = Math.random() * 6 - 3;
        this.speedY = Math.random() * -6 - 2;
        this.color = '#4CAF50';
        this.life = 1.0;
    }
    update() {
        this.x += this.speedX;
        this.y += this.speedY;
        this.speedY += 0.2; // gravity
        this.life -= 0.02;
    }
    draw() {
        ctx.fillStyle = `rgba(76, 175, 80, ${this.life})`;
        ctx.beginPath();
        ctx.arc(this.x, this.y, this.size, 0, Math.PI * 2);
        ctx.fill();
    }
}

function createParticles(x, y) {
    for (let i = 0; i < 30; i++) particlesArray.push(new Particle(x, y));
}

function animateParticles() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    for (let i = 0; i < particlesArray.length; i++) {
        particlesArray[i].update();
        particlesArray[i].draw();
        if (particlesArray[i].life <= 0) {
            particlesArray.splice(i, 1);
            i--;
        }
    }
    requestAnimationFrame(animateParticles);
}
animateParticles();

// --- Manager ---
function loadManager() {
    const pSelect = document.getElementById('new-habit-plant');
    pSelect.innerHTML = '';
    
    // Base plants available to everyone immediately
    const basePlants = ['rose', 'lily', 'tulip', 'fern', 'mushroom', 'bamboo'];
    const unlocked = Array.from(new Set([...basePlants, ...(userStats.unlocked_plants || [])]));
    
    unlocked.forEach(pt => {
        if(PLANT_TYPES[pt]) {
            const opt = document.createElement('option');
            opt.value = pt;
            opt.innerText = `${PLANT_TYPES[pt].emoji} ${PLANT_TYPES[pt].name}`;
            pSelect.appendChild(opt);
        }
    });

    const list = document.getElementById('manage-habits-list');
    list.innerHTML = '';
    currentHabits.forEach(h => {
        const div = document.createElement('div');
        div.className = 'habit-card';
        div.innerHTML = `
            <div class="habit-info">
                <span class="habit-name">${h.name}</span>
                <span class="habit-streak">${h.category} | ${h.plant_type}</span>
            </div>
            <button class="btn-danger" onclick="deleteHabit(${h.id})">Delete</button>
        `;
        list.appendChild(div);
    });
}

async function handleAddHabit(e) {
    e.preventDefault();
    const n = document.getElementById('new-habit-name').value;
    const c = document.getElementById('new-habit-cat').value;
    const p = document.getElementById('new-habit-plant').value;
    
    await API.addHabit(n, c, p);
    document.getElementById('new-habit-name').value = '';
    currentHabits = await API.getHabits();
    loadManager();
}

async function deleteHabit(id) {
    if(confirm('Delete this habit map and plant?')) {
        await API.deleteHabit(id);
        currentHabits = await API.getHabits();
        loadManager();
    }
}

// --- Stats Page ---
function loadStats() {
    const heatmap = document.getElementById('heatmap');
    heatmap.innerHTML = '';
    // Mock heatmap data for 30 days based on completion history
    // A robust impl would check actual dates against the last 30 days
    for (let i = 0; i < 90; i++) {
        const cell = document.createElement('div');
        let intensity = 0;
        if(Math.random() > 0.5) intensity = Math.floor(Math.random() * 4);
        cell.className = `heat-cell ${intensity > 0 ? 'active-heat-'+intensity : ''}`;
        heatmap.appendChild(cell);
    }
}

function exportCSV() {
    let csv = "Habit Name,Category,Streak,Plant Stage,Completed Today\n";
    currentHabits.forEach(h => {
        csv += `${h.name},${h.category},${h.streak},${h.plant_stage},${h.completed_today}\n`;
    });
    
    const blob = new Blob([csv], { type: 'text/csv' });
    const url = window.URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = 'habit_garden_stats.csv';
    a.click();
}

function exportData() {
    const data = localStorage.getItem('habit_garden_data') || '{"users":[]}';
    const blob = new Blob([data], { type: 'application/json' });
    const url = window.URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = 'habit_garden_backup.json';
    a.click();
}

function importData(event) {
    const file = event.target.files[0];
    if (!file) return;
    const reader = new FileReader();
    reader.onload = function(e) {
        try {
            const json = JSON.parse(e.target.result);
            if (json && json.users) {
                localStorage.setItem('habit_garden_data', JSON.stringify(json));
                alert('Data imported successfully! The page will now reload.');
                window.location.reload();
            } else {
                alert('Invalid backup file format.');
            }
        } catch (err) {
            alert('Error parsing the file.');
        }
    };
    reader.readAsText(file);
    event.target.value = ''; // reset
}

// --- Avatar Logic ---
let currentAvatar = localStorage.getItem('hg_avatar') || '🦉';
const speechPhrases = [
    "Keep it up! 🪴",
    "Time to grow some habits!",
    "What a great streak!",
    "Looking forward to today's goals!",
    "You're doing amazing! ✨",
    "Don't let your plants wilt!"
];

function initAvatar() {
    document.getElementById('avatar-emoji').innerText = currentAvatar;
    setTimeout(() => say("Welcome back! Ready to grow?"), 500);
}

function setAvatar(emoji) {
    currentAvatar = emoji;
    localStorage.setItem('hg_avatar', emoji);
    initAvatar();
    say("Nice to meet you! I'm your new buddy.");
}

function setCustomAvatar() {
    const val = document.getElementById('custom-avatar-input').value.trim();
    if (val) {
        setAvatar(val);
        document.getElementById('custom-avatar-input').value = '';
    }
}

let speechTimeout = null;
function say(msg) {
    const bubble = document.getElementById('avatar-speech');
    bubble.innerText = msg;
    bubble.classList.add('show');
    clearTimeout(speechTimeout);
    speechTimeout = setTimeout(() => {
        bubble.classList.remove('show');
    }, 4000);
}

function pokeAvatar() {
    const randomMsg = speechPhrases[Math.floor(Math.random() * speechPhrases.length)];
    say(randomMsg);
}

// --- Notifications ---
function enableNotifications() {
    if (!("Notification" in window)) {
        alert("This browser does not support desktop notification");
    } else if (Notification.permission === "granted") {
        alert("Notifications are already enabled!");
        scheduleReminder();
    } else if (Notification.permission !== "denied") {
        Notification.requestPermission().then((permission) => {
            if (permission === "granted") {
                say("Yay! I'll remind you to check your plants.");
                scheduleReminder();
            }
        });
    }
}

function scheduleReminder() {
    // For demo purposes, we will trigger a sample notification in 5 seconds
    setTimeout(() => {
        new Notification("🌱 Habit Garden Reminder!", {
            body: "Your plants need water! Complete a habit to help them grow.",
            icon: "https://raw.githubusercontent.com/twitter/twemoji/master/assets/72x72/1f331.png"
        });
    }, 5000); // 5 seconds test reminder!
}
