#Hello, This is Arif with his first complete project in GitHub. 
# Task Tracker CLI

A command-line task management application built with **C++17**. It allows users to create, update, delete, and track tasks while storing data persistently in a local JSON file.

This project is based on the [roadmap.sh Task Tracker challenge](https://roadmap.sh/projects/task-tracker).

## Features

- Add new tasks
- Update task descriptions
- Delete tasks
- Mark tasks as `done` or `in-progress`
- List all tasks or filter by status
- Automatically assign task IDs
- Store tasks in `tasks.json`
- Record task creation and modification timestamps in UTC
- Validate positive integer task IDs
- Handle missing and malformed JSON data
- Use temporary-file writing to reduce the risk of data loss

## Technologies

- C++17
- Windows / PowerShell
- MSYS2 UCRT64 g++
- nlohmann/json (single-header library)
- Windows API for file replacement

## Installation

### Prerequisites

- Windows
- A C++17-compatible compiler, such as MSYS2 UCRT64 g++

### Build

Clone the repository:

```powershell
git clone https://github.com/YOUR-USERNAME/YOUR-REPOSITORY.git
cd YOUR-REPOSITORY
```

Compile:

```powershell
g++ -std=c++17 main.cpp -o task-cli
```

## Usage

Add a task:

```powershell
.\task-cli.exe add "Buy groceries"
```

List all tasks:

```powershell
.\task-cli.exe list
```

Update a task:

```powershell
.\task-cli.exe update 1 "Buy groceries and milk"
```

Mark a task as in progress:

```powershell
.\task-cli.exe mark-in-progress 1
```

Mark a task as done:

```powershell
.\task-cli.exe mark-done 1
```

Delete a task:

```powershell
.\task-cli.exe delete 1
```

Filter tasks by status:

```powershell
.\task-cli.exe list todo
.\task-cli.exe list in-progress
.\task-cli.exe list done
```

## Data Storage

Tasks are stored locally in `tasks.json`, which is created automatically when the first task is added.

Example:

```json
[
  {
    "id": 1,
    "description": "Buy groceries",
    "status": "todo",
    "createdAt": "2026-11-11T13:37:54Z",
    "updatedAt": "2026-11-11T13:37:54Z"
  }
]
```

The application supports older task records without timestamp fields. Missing timestamps are loaded as empty strings.

The task data file is excluded from Git tracking using `.gitignore`.

## Error Handling and Reliability

The application:

- Rejects invalid positive integer task IDs
- Returns nonzero exit codes for tested invalid commands and missing tasks
- Rejects malformed JSON instead of silently overwriting it
- Supports startup when no `tasks.json` file exists
- Writes new JSON to a temporary file before replacing the existing data file

## Known Limitations

- Developed for Windows; file replacement uses Windows-specific APIs.
- Uses nlohmann/json despite the original roadmap.sh challenge specifying no external libraries. Therefore, this version is not strictly compliant with that restriction.
- File replacement reduces the risk of data loss but does not guarantee durability during crashes, power failures, or all filesystem errors.
- Task IDs are assigned using the maximum existing ID plus one, so a deleted highest ID may be reused.
- Tasks are stored relative to the current working directory.
- Concurrent program instances are not supported.
- Output is primarily intended for terminal use; listing displays task descriptions rather than a formatted table.

## Project Purpose

This project was developed to practice:

- Command-line argument processing using `argc` and `argv`
- C++ structures and vectors
- Loops, conditions, and functions
- File input/output
- JSON serialization and deserialization
- Input validation and error handling
- Persistent storage and testing
- Git and GitHub workflows

## License

The application source code has no declared license. The bundled `json.hpp` library retains its own upstream licensing terms.
