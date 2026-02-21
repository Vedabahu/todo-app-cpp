# Todo App - C++ Backend API

A RESTful backend API for a Todo application, built with **C++23**, **[Crow](https://crowcpp.org/)** (a fast HTTP micro-framework), and **SQLite3** for persistence. The project currently provides user registration and lays the groundwork for full CRUD operations on todos.

> **Status: Work in Progress** - Basic Auth middleware and all Todo endpoints are not yet implemented.

---

## Table of Contents

- [Why C++?](#why-c)
- [Features](#features)
- [Architecture](#architecture)
- [Tech Stack](#tech-stack)
- [Prerequisites](#prerequisites)
- [Getting Started](#getting-started)
  - [1. Clone the Repository](#1-clone-the-repository)
  - [2. Install Dependencies via vcpkg](#2-install-dependencies-via-vcpkg)
  - [3. Configure with CMake](#3-configure-with-cmake)
  - [4. Build](#4-build)
  - [5. Run](#5-run)
- [Dev Container](#dev-container)
- [API Reference](#api-reference)
- [Project Structure](#project-structure)
- [Database Schema](#database-schema)
- [Roadmap](#roadmap)

---

## Why C++?

Firstly, I am trying to learn C/C++ in complete depth for my goal of becoming a systems engineer. 

Secondly, most backend services are written in JavaScript (Node.js), Python, or Java - languages that trade raw performance for developer velocity. This project deliberately uses C++ to explore what a truly low-overhead web API looks like:

- **Near-zero runtime overhead** - No garbage collector pauses, no JIT warm-up, no interpreter. Every request is handled by compiled machine code.
- **Predictable memory usage** - Stack allocation and RAII mean memory is managed at compile time, not at runtime.
- **Fine-grained control** - You decide exactly what happens at every layer, from I/O to hashing to thread scheduling.
- **Crow is impressively ergonomic** - Despite being C++, the routing DSL feels as expressive as Express.js or Flask.
- **Great learning exercise** - Understanding how to wire HTTP, SQLite, OpenSSL and threading together without a runtime framework builds strong systems intuition.

### Potential Downsides

- **Longer compile times** - A full rebuild with CMake + vcpkg dependencies can take several minutes on first run.
- **Manual memory safety** - Without smart-pointer discipline, dangling pointers and use-after-free bugs are possible. The project uses RAII throughout, but vigilance is required.
- **SQLite access may not be thread-safe** - Each request opens a fresh SQLite connection (per the current `Database` design), which avoids shared-state bugs but adds syscall overhead. A connection pool would be needed for high throughput.
- **SHA-256 for password hashing** - SHA-256 is a fast cryptographic hash, not a password-hardening function. It was chosen intentionally for simplicity in this project. Production systems should use Argon2 or bcrypt (adding either as an optional component is on the roadmap).
- **Steeper onboarding** - Contributors familiar only with JavaScript or Python will need time to get comfortable with the build system, templates, and memory model.
- **Smaller ecosystem** - Fewer ready-made middleware and plug-and-play libraries compared to the Node.js or Python ecosystems.

---

## Features

- **User Registration** - Accepts a username and password; stores the user with a SHA-256 hashed password and a UUID primary key.
- **HTTP Basic Authentication** - Protected routes read the `Authorization` header, decode the Base64 `username:password` credential, and verify against the stored hash.
- **SQLite3 Persistence** - Lightweight embedded database with WAL journal mode for improved concurrency.
- **GZIP Compression** - All responses are served with GZIP compression.
- **Multithreaded** - Crow runs in multithreaded mode for concurrent request handling.
- **Layered Architecture** - Clean separation between controllers, services, repositories, models, and utilities.

---

## Architecture

```
HTTP Request
     │
     ▼
┌─────────────────┐
│   Controllers   │  Parses HTTP requests, validates input, returns responses
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│    Services     │  Business logic (hashing, UUID generation, validation)
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Repositories   │  SQL queries; abstracts all database access
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│    Database     │  SQLite3 connection management + schema initialization
└─────────────────┘
```

---

## Tech Stack

| Component       | Technology                          |
|-----------------|-------------------------------------|
| Language        | C++23                               |
| HTTP Framework  | [Crow](https://crowcpp.org/) (git submodule) |
| Database        | SQLite3 (via vcpkg)                 |
| Hashing         | OpenSSL (SHA-256, via vcpkg)        |
| Compression     | zlib (via vcpkg)                    |
| Async I/O       | ASIO (via vcpkg, standalone)        |
| Build System    | CMake 3.25+ with Ninja              |
| Dependency Mgr  | vcpkg                               |

---

## Prerequisites

Make sure the following are installed on your system before building locally:

| Tool         | Minimum Version | Notes                                  |
|--------------|-----------------|----------------------------------------|
| GCC / Clang  | GCC 13 / Clang 17 | Must support C++23                  |
| CMake        | 3.25            |                                        |
| Ninja        | Any recent      | Used as the CMake generator            |
| vcpkg        | Latest          | Set `VCPKG_ROOT` environment variable  |
| OpenSSL      | 3.x             | Can also be satisfied via vcpkg        |
| git          | Any             | Required to clone with submodules      |

> **Debian/Ubuntu quick install:**
> ```bash
> sudo apt update && sudo apt install -y \
>     build-essential git cmake ninja-build \
>     libssl-dev zlib1g-dev libsqlite3-dev
> ```

---

## Getting Started

### 1. Clone the Repository

Clone recursively to also fetch the Crow submodule:

```bash
git clone --recurse-submodules https://github.com/Vedabahu/todo-app-cpp.git
cd todo-app-cpp
```

If you already cloned without `--recurse-submodules`, initialize the submodule manually:

```bash
git submodule update --init --recursive
```

### 2. Install Dependencies via vcpkg

Ensure vcpkg is installed and the `VCPKG_ROOT` environment variable points to your vcpkg installation:

```bash
export VCPKG_ROOT=/path/to/vcpkg   # e.g. /usr/local/vcpkg
```

vcpkg will automatically install the required packages (declared in `vcpkg.json`) during the CMake configure step:

- `zlib`
- `openssl`
- `asio` (standalone)
- `sqlite3` (with unicode support)

### 3. Configure with CMake

Use the bundled `vcpkg` CMake preset (defined in `CMakePresets.json`), which automatically sets the vcpkg toolchain file and uses Ninja as the generator, with output going to `./build/`:

```bash
cmake --preset vcpkg
```

To explicitly set a build type (default is `Debug`):

```bash
cmake --preset vcpkg -DCMAKE_BUILD_TYPE=Release
```

### 4. Build

```bash
cmake --build build
```

Or, using Ninja directly:

```bash
ninja -C build
```

To build with multiple CPU cores:

```bash
cmake --build build --parallel $(nproc)
```

The compiled binary will be output to:

```
build/todo_api
```

### 5. Run

```bash
./build/todo_api
```

The server starts on **port `18080`** by default. You can verify it's running with:

```bash
curl http://localhost:18080/register \
  -X POST \
  -H "Content-Type: application/json" \
  -d '{"username": "Vedabahu", "password": "secret123"}'
```

---

## Dev Container

This project includes a fully configured VS Code **Dev Container** for a zero-setup development environment.

**Requirements:** [VS Code](https://code.visualstudio.com/) + [Dev Containers extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) + Docker.

The container is based on `mcr.microsoft.com/devcontainers/cpp:1-debian12` and automatically installs:
- vcpkg (latest)
- Ninja (via asdf)

**To open in the dev container:**

1. Open the project folder in VS Code.
2. Press `Ctrl+Shift+P` → **"Dev Containers: Reopen in Container"**.
3. Wait for the container to build and start.
4. Follow the [Getting Started](#getting-started) steps above from inside the container terminal.

---

## API Reference

> Base URL: `http://localhost:18080`

### Authentication

All endpoints **except** `POST /register` require HTTP Basic Authentication.

The client must send credentials encoded as `username:password` in Base64 inside the `Authorization` header:

```
Authorization: Basic <base64(username:password)>
```

**Example** - credentials `Vedabahu:secret123`:

```bash
curl http://localhost:18080/todos \
  -H "Authorization: Basic VmVkYWJhaHU6c2VjcmV0MTIz"
```

You can generate the header value with:

```bash
echo -n "Vedabahu:secret123" | base64
```

The server decodes the header, splits on `:`, hashes the password with SHA-256, and compares it against the stored hash. No session tokens or cookies are used.

---

### `POST /register`

Register a new user account. Does **not** require authentication.

**Request Body** *(application/json)*:
```json
{
  "username": "Vedabahu",
  "password": "secret123"
}
```

**Responses:**

| Status | Meaning                                  |
|--------|------------------------------------------|
| `201`  | User created successfully                |
| `400`  | Missing or empty fields, or invalid JSON |
| `409`  | Username already exists                  |

---

### Todos *(Not yet implemented)*

All todo endpoints will require a valid `Authorization: Basic ...` header.

| Method   | Endpoint         | Description              |
|----------|------------------|--------------------------|
| `POST`   | `/todos`         | Create a new todo        |
| `GET`    | `/todos`         | List all todos for user  |
| `GET`    | `/todos/:id`     | Get a specific todo      |
| `PUT`    | `/todos/:id`     | Update a todo            |
| `DELETE` | `/todos/:id`     | Delete a todo            |

---

## Project Structure

```
cpp_backend/
├── .devcontainer/          # VS Code Dev Container configuration
│   ├── Dockerfile
│   └── devcontainer.json
├── external/
│   └── Crow/               # Crow HTTP framework (git submodule)
├── src/
│   ├── main.cpp            # Entry point: wires up DB, services, routes, and starts server
│   ├── controllers/
│   │   ├── auth_controller.hpp
│   │   └── auth_controller.cpp   # Defines /register route
│   ├── database/
│   │   ├── database.hpp
│   │   └── database.cpp          # SQLite connection management & schema init
│   ├── models/
│   │   └── user.hpp              # User struct (id, username, password_hash)
│   ├── repositories/
│   │   ├── user_repository.hpp
│   │   └── user_repository.cpp   # SQL queries for user CRUD
│   ├── services/
│   │   ├── auth_service.hpp
│   │   └── auth_service.cpp      # Registration business logic
│   └── utils/
│       ├── hashing.hpp / .cpp    # SHA-256 via OpenSSL
│       └── uuid.hpp / .cpp       # UUID v4 generation
├── data/                   # Runtime database files (auto-created, gitignored)
│   └── todo.db
├── CMakeLists.txt
├── CMakePresets.json       # Defines the "vcpkg" configure preset
└── vcpkg.json              # vcpkg manifest with dependency declarations
```

---

## Database Schema

The database file is stored at `data/todo.db` (created automatically on first run).

```sql
-- Users table
CREATE TABLE IF NOT EXISTS users (
    id            TEXT PRIMARY KEY,    -- UUID v4
    username      TEXT UNIQUE NOT NULL,
    password_hash TEXT NOT NULL        -- SHA-256 hex digest
);

-- Todos table
CREATE TABLE IF NOT EXISTS todos (
    id         TEXT PRIMARY KEY,       -- UUID v4
    user_id    TEXT NOT NULL,
    title      TEXT NOT NULL,
    completed  INTEGER NOT NULL DEFAULT 0,  -- 0 = false, 1 = true
    created_at TEXT NOT NULL,
    updated_at TEXT NOT NULL,
    FOREIGN KEY(user_id) REFERENCES users(id)
);
```

> WAL (Write-Ahead Logging) mode is enabled for better concurrent read/write performance.

---

## Roadmap

- [ ] **Basic Auth middleware** - Decode `Authorization` header, verify credentials, gate all todo routes
- [ ] **Todo CRUD** - Full create, read, update, delete for todos per authenticated user
- [ ] **Input validation** - Length limits, sanitization
- [ ] **Logging** - Structured request/response logging
- [ ] **Tests** - Unit and integration tests
- [ ] **Docker Compose** - Containerized deployment
- [ ] **Password hardening** *(optional)* - Swap SHA-256 for Argon2 or bcrypt

---

## License

This project is licensed under the **GNU General Public License v3.0**. See [LICENSE](LICENSE) for full terms.
