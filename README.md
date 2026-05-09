# 🐔 Chicken Invaders (C++ Arcade Game)

A 2D arcade-style survival game built in **C++**, featuring classic chicken-dodging gameplay, difficulty scaling, save/load functionality, and a structured menu system.

---

## 📌 Overview

**Chicken Invaders** is a fast-paced competitive arcade game where the player must survive by dodging eggs dropped by chickens above. The game includes multiple difficulty levels, a save system, and a simple but structured menu-driven interface.

This project is built using **Programming Fundamentals (PF) concepts**, focusing on logic building, state management, and file handling.

---

## 🎮 Features

### 🧭 Game Menu
- Start Game
- Difficulty Selection (Easy / Medium / Hard)
- Save & Load System
- Instructions Screen

### 🐣 Gameplay
- Chickens continuously drop eggs from above
- Player must dodge incoming eggs to survive
- Increasing challenge based on difficulty level
- Competitive survival-based scoring system

### 💾 Save System
- Game state stored in `save.txt`
- Resume gameplay from last saved session

### ⚙️ Difficulty System
- Controls:
  - Chicken movement speed
  - Egg drop frequency
  - Overall game difficulty scaling

### 🧠 Technical Highlights
- Built using **C++ (PF-level logic)**
- Uses:
  - Loops & conditionals
  - Arrays / state tracking
  - File handling
  - Menu-driven architecture

---

## 🗂 Project Structure

ChickenInvaders/
│
├── Source/
│   └── main.cpp
│
├── Run/
│   ├── chickeninvader.exe
│   ├── difficulty.txt
│   ├── save.txt
│   └── assets/
│       ├── (game images / sounds / sprites)
│
└── README.md

---

## 🎮 Controls

| Key | Action |
|-----|--------|
| W / A / S / D | Move player |
| Arrow Keys (↑ ↓ ← →) | Alternative movement controls |
| Space | Action / Shoot (if enabled) |
| ESC | Pause / Exit / Return to menu |

---

## 🚀 How to Run

1. Open the `Run` folder  
2. Ensure all required files are present:
   - `chickeninvader.exe`
   - `difficulty.txt`
   - `save.txt`
   - `assets/` folder
3. Run `chickeninvader.exe`

---

## 📖 Instructions

- Avoid eggs dropped by chickens above
- Survive as long as possible to increase score
- Select difficulty before starting for different challenges
- Use save/load feature to continue progress anytime

---

## 👨‍💻 Author

**Hammad Anis**

---

## 📄 License

This project is developed for educational purposes only.
