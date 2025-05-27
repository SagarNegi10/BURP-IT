
# 🍔 BURPIT - Food Delivery & Order Management System 

**BURPIT** is a feature-rich C-based food delivery and order management system designed to simulate a real-world food delivery application. This console-based project supports both **admin** and **customer** functionalities, making it a great learning project for understanding file handling, data structures, authentication, and dynamic memory management in C.
---
## 🔧 Project Overview
**BURPIT** enables:
- Seamless **user registration and login**
- Intuitive **menu browsing and management**
- Robust **order placement, cancellation, and processing**
- Real-time **delivery route optimization using Dijkstra’s Algorithm**
- Secure **payment handling**
- Clear **order history and sales reporting**
---
## 🚀 Features

### 🔐 User Authentication
- Register as a new user or admin (admin key required).
- Secure password input using hidden characters (`*`).
- Persistent user storage via file handling.

### 🧑 Customer Panel
- Browse menu items with styled tabular display.
- Place food orders and pay via **Cash** or **Card**.
- Cancel or undo recently cancelled orders.
- Learn about BURPIT via an animated "About Us" section.

### 👨‍💼 Admin Panel
- **Menu Management**:
  - Add, update, and delete menu items.
- **Order Management**:
  - View, process, cancel, and undo customer orders.
  - View complete order history.
- **Delivery Route System**:
  - Add delivery locations and routes.
  - View distance matrix.
  - Calculate shortest delivery route using **Dijkstra's Algorithm**.
- **Sales Reporting**:
  - View all payments with a total sales summary.

### 🖥️ Console UI Enhancements
- Animated loading and welcome screens.
- Styled banners and color-coded tables.
- Center-aligned console output for visual appeal.
---

## ⚙️ Technologies Used

- **Language:** C (with Windows API for console handling)
- **Compiler:** GCC / Turbo C++ (Windows environment)
- **Data Storage:** File handling using `.txt` files

### 🔍 Algorithm
- **Dijkstra’s Algorithm** – For delivery route optimization

### 🧠 Data Structures

#### 🔹 Linked List
- **Used for:**
  - Dynamic menu items
  - Order history tracking

#### 🔹 Queue / Priority Queue
- **Used for:**
  - Processing food orders in FIFO manner
  - Prioritizing urgent deliveries

#### 🔹 Stack
- **Used for:**
  - Undo functionality for recently cancelled orders

#### 🔹 Graph (Adjacency Matrix)
- **Used for:**
  - Mapping delivery locations and distances
  - Finding optimal routes with Dijkstra's Algorithm


---

## 🧪 How to Run

To get started with BURPIT on your local machine:

### ✅ Prerequisites
- Windows system (uses `windows.h`)
- A C compiler (GCC / Turbo C++ / Code::Blocks)

### 1. Clone the Repository

- git clone https://github.com/SagarNegi10/BURP-IT.git

- cd burpit

### 2. Create Required Data Files

- mkdir data touch data/users.txt data/menu.txt data/orders.txt data/payments.txt data/routes.txt data/history.txt

### 3. Compile the Code

- gcc Main.c -o Burpit.exe

### 4. Run the Application

- ./Burpit.exe

---
## 👨‍💻 Authors

Developed under the guidance of **Ms. Shobha Aswal**

### Team ROCKET

- **Sagar Negi** – 24711207 – sagarnegi13.0@gmail.com  
- **Mohammad Kaif** – 24711363 – kaiiff2802@gmail.com  
- **Ritik Uniyal** – 24711185 – ritikuniyal9999@gmail.com  
- **Rita Rathore** – 24712183 – Ritarathore97772@gmail.com
---

## 📜 License

This project is intended for **educational purposes only**.  
You are welcome to **modify** and **enhance** it for learning or personal development.

---

