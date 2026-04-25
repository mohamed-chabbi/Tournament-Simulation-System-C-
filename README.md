# Tournament Simulation System (C)

## Description
A console-based C program that simulates a dynamic player tournament using multiple queues and linked lists.  
Players compete in matches governed by different strategies, and their performance determines their progression through various queues and rankings.

---

## Features
- Dynamic player creation
- Multiple queues (F, F1, F3)
- Linked list ranking system (winners and losers)
- Two game strategies:
  - Strategy I: sum of digits condition
  - Strategy II: PGCD-based condition
- Automatic tournament simulation
- Player statistics tracking
- Ranking system (top performers)

---

## Game Logic
Each match is simulated between two players:
- Players are selected from different queues based on priority rules
- A random-based system determines scores
- Winners and losers are redistributed into different queues
- Special conditions can move players to ranking lists (LG, LP)

---

## Data Structures Used
- Linked Lists
- Queues
- Dynamic memory allocation

---

## Compilation & Execution
```bash
gcc main.c -o tournament
./tournament
