MediCore — Hospital Management System

A fully-featured desktop hospital management system built with **C++** and **SFML 2.6.1**, featuring a graphical user interface, role-based login, appointment scheduling, billing, prescriptions, and admin reporting.

---

Features

Patient Portal
- Book appointments by specialization, doctor, date, and time slot
- Cancel appointments with automatic refund
- View appointment history with status indicators
- View medical records and prescriptions
- Pay bills and top up account balance

Doctor Portal
- View today's scheduled appointments
- Mark appointments as completed or no-show
- Write and save prescriptions
- View patient medical history (access-controlled)

Admin Panel
- Add / remove doctors and patients
- View all patients, doctors, and appointments
- View and filter unpaid bills with overdue detection
- Discharge patients (archives record, clears related data)
- Daily report with revenue, appointment stats, and doctor summaries
- Security log with failed login tracking

Security
- Role-based login (Patient / Doctor / Admin)
- Account lockout after 3 failed attempts
- All failed logins logged to `data/security_log.txt`

---
## Quick Run

1. Download release or build project
2. Ensure `Run/` folder is intact
3. Run `MediCore.exe`

Project Structure

```
MediCore.zip
│
├── Source/
│   ├── .cpp files
│   ├── .h files
│   └── data/
│
├── Run/
│   ├── MediCore.exe
│   ├── sfml-graphics-2.dll
│   ├── sfml-window-2.dll
│   ├── sfml-system-2.dll
│   ├── sfml-audio-2.dll
│   ├── arial.ttf
│   ├── Background.ogg
│   ├── FAHH.ogg
│   └── data/
│
└── README.md

---

 Requirements

| Requirement | Details |
|---|---|
| Compiler | MSVC / GCC / Clang (C++17) |
| SFML | Version **2.6.1** |
| OS | Windows 10/11 (tested), Linux (compatible) |
| Font | `arial.ttf` in the executable directory |

---

Building from Source

### Windows (Visual Studio)

1. Install [SFML 2.6.1](https://www.sfml-dev.org/download/sfml/2.6.1/)
2. Create a new Visual Studio C++ project
3. Add all `.cpp` and `.h` files to the project
4. Configure project properties:
   - **Include Directories** → `<SFML>/include`
   - **Library Directories** → `<SFML>/lib`
   - **Linker → Additional Dependencies**:
     ```
     sfml-graphics.lib
     sfml-window.lib
     sfml-system.lib
     sfml-audio.lib
     ```
5. Copy SFML `.dll` files to the output directory
6. Build and run

### Windows (g++ / MinGW)

```bash
g++ -o MediCore GuiMain.cpp Patient.cpp Doctor.cpp Admin.cpp Appointment.cpp Bill.cpp Prescription.cpp FileHandler.cpp Validator.cpp Helpers.cpp Button.cpp InputField.cpp -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -std=c++17
```

---

Running the Executable

1. Make sure these files are in the **same folder** as `MediCore.exe`:
   - `arial.ttf`
   - `Background.ogg`
   - `FAHH.ogg`
   - `data/` folder (with all `.txt` files, can be empty on first run except `admin.txt`)

2. The `data/admin.txt` file must exist with admin credentials in this format:
   ```
   1 AdminName password
   ```

3. Run `MediCore.exe` inside run folder. 

---

Controls

| Input | Action |
|---|---|
| Mouse | Navigate all menus and buttons |
| Arrow Up / Down | Scroll through lists |
| Enter | Confirm login |
| Left Ctrl | Force logout / return to main menu |

---

 Author

**Hammad Anis**  
MediCore v1.0

---

License

This project is for educational purposes.
