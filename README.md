# CampusConnect 🎓

A console-based Campus Management & Student Collaboration System built in C++ using multiple core Data Structures & Algorithms (DSA) concepts.

CampusConnect helps students navigate campus life by providing:

- Campus navigation with shortest path calculation
- Room booking management
- Event scheduling
- Student matching for study groups
- Notifications and activity logs
- Search autocomplete system
- Persistent file storage

This project was designed as a practical implementation of real-world DSA applications in a university environment.

---

# 🚀 Features

## Campus Navigation
- Campus locations stored using an Adjacency List Graph
- Uses Dijkstra’s Algorithm to find shortest routes between locations

## Room Booking System
- Room booking requests managed using a Min-Heap Priority Queue
- Higher priority bookings are processed first

## Event Management
- Events stored chronologically using a Binary Search Tree (BST)

## Student Management
- Fast student lookup using Hash Tables
  - Email → Student mapping
  - ID → Student mapping

## Autocomplete Search
- Campus locations and student names searchable using a Trie

## Undo / Redo System
- Booking undo functionality implemented using a Linked-List Stack

## Notification System
- Notifications managed with a Circular Queue

## Activity Logging
- Bidirectional activity history using a Doubly Linked List

## Persistent Storage
Data is automatically saved into:
- `students.dat`
- `bookings.dat`
- `events.dat`
- `campus_log.txt`

---

# 🧠 Data Structures Used

| Data Structure | Purpose |
|----------------|---------|
| Graph (Adjacency List) | Campus map navigation |
| Min-Heap | Booking priority queue |
| Binary Search Tree | Event scheduling |
| Hash Table | Student indexing |
| Trie | Autocomplete search |
| Stack (Linked List) | Undo/Redo system |
| Circular Queue | Notifications |
| Doubly Linked List | Activity logs |

---

# 📂 Project Structure

```bash
CampusConnect/
│
├── CampusConnect_1.cpp
├── students.dat
├── bookings.dat
├── events.dat
├── campus_log.txt
└── README.md
```

# 🖥️ Main Menu

```text
ACCOUNT
[1] Student Login
[2] Admin Login
[3] Register New Student

CAMPUS
[4] Campus Map & Navigation
[5] Location Search (Autocomplete)

BOOKINGS
[6] Book a Room
[7] Undo Last Booking
[8] My Bookings

COMMUNITY
[9] Find Study Partners
[10] Campus Events

SYSTEM
[11] Dashboard / Stats
[12] Notifications
[13] My Activity Log
[14] Exit
```

---

# 🔐 Admin Credentials

```text
Username: admin
Password: admin123
```

---

# 📌 Example Functionalities

## Shortest Path Navigation
Find the shortest route between:
- Library
- Cafeteria
- Auditorium
- Sports Complex
- CS Department

using Dijkstra’s Algorithm.

---

## Study Partner Matching
Students are matched based on:
- Department
- Academic year
- Free time
- Subjects
- Interests

---

## Booking Priority
Bookings are processed based on priority:
- `1 = Highest`
- `5 = Lowest`

Implemented using a Min-Heap.

---

# 📈 Learning Objectives

This project demonstrates practical implementation of:

- Object-Oriented Programming (OOP)
- Dynamic Memory Management
- File Handling
- Custom Data Structures
- Algorithm Design
- Modular Programming
- Console UI Design

---

# 🛠️ Future Improvements

Possible future upgrades:
- GUI version using Qt or SFML
- Database integration (MySQL / SQLite)
- Authentication system
- Real-time notifications
- Multi-user networking
- Web-based version
- Booking conflict detection
- Advanced event filtering

---

# 👨‍💻 Author

Developed as a Data Structures & Algorithms Project in C++.

---
