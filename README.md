# Custom Programming Language & Compiler using LLVM

This repository contains the implementation of a simple programming language and its compiler, built with **LLVM** in **C++**. The project was developed as a group final project for the **Compiler Design** course in the 5th semester.

## 📖 Language Features

Our custom language provides a set of essential programming constructs, including:

### ✅ Data Types
- `int`
- `float`
- `bool`
- `var` (generic)

### ✅ Operators
- **Arithmetic Operators:** `+`, `-`, `*`, `/`, `%`
- **Comparison Operators:** `==`, `!=`, `<`, `>`, `<=`, `>=`
- **Logical Operators:** `&&`, `||`, `!`
- **Bitwise Operators:** `&`, `|`, `^`, `~`, `<<`, `>>`
- **Assignment Operators:** `=`, `+=`, `-=`, etc.
- **Unary Operators**
- **Ternary Operator**

### ✅ Type Casting
Ability to cast between supported data types.

### ✅ Mathematical Functions
- `min(a, b)`
- `max(a, b)`
- `mean(a, b)`
- `sqrtN(a, n)` — N-th root of a number

### ✅ Control Flow
- **Conditions:** `if`, `else if`, `else`
- **Switch Case**
- **Loops:** `for`, `while`
  - Supports `break` and `continue`

### ✅ Other Features
- **Variable and Constant Declaration**
- **Comments**
- **Print Function**

---

## 🛠️ Technologies Used
- **C++**
- **LLVM** (as the backend for code generation)
- **Constant Propagation** (Compiler Optimization)
- **Constant Folding** (Compiler Optimization)
---
