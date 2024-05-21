# Operating System Simulator

This project is a simulation of an operating system, designed to mimic the behavior of process scheduling, resource management, and inter-process communication.

Google Docs link for Report
https://docs.google.com/document/d/1CEQ_GfQ3AWDeaZaWLFKrwmf8eHq1TsfIpYiQt3h58UE/edit

## Table of Contents

- [Installation](#installation)
- [Usage](#usage)
- [Folder Structure](#folder-structure)

## Installation

Clone the repository to your local machine:

```bash
git clone https://github.com/yourusername/Operating-System-Simulator.git
```

## Usage

**Milestone 1**

- creates and handles thread scheduling algorithms.

To compile and run the program:

```bash
gcc main.c && sudo ./a.out -pf99
```

**Milestone 2**

- Create processes and mutexes.
- Allocate memory for all PCBs and their program variables and instructions.
- Resource management.
- Round Robin for process scheduling.

Navigate to the milestone 2 directory:

```bash
cd milestone\ 2
```

Compile the program:

```bash
gcc -o finalProgram main.c ProcessCreation.c resourceManager.c shared.c Queue.c
```

Run the program:

```bash
./finalProgram
```

## Folder Structure

```
.
├── main.c  # Main file for Milestone 1
├── milestone 2/  # Directory for Milestone 2
│   ├── main.c  # Main file for Milestone 2
│   ├── ProcessCreation.c  # process creation and memory allocation functions
│   ├── Program_1.txt  # Text file for the first program (process)
│   ├── Program_2.txt  # Text file for the second program (process)
│   ├── Program_3.txt  # Text file for the third program (process)
│   ├── Queue.c  # queue management functions
│   ├── resourceManager.c  # resource management functions
│   ├── shared.c  # initializing shared variables
│   └── shared.h  # Header file for shared function declarations and structures
└── README.md  # Documentation file
```
