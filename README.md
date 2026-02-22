# Todo App - C++ Backend API

A RESTful backend API for a Todo application, built with **C++23**, **[Crow](https://crowcpp.org/)** (a fast HTTP micro-framework), and **SQLite3** for persistence.

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
- [Docker](#docker)
- [Dev Container](#dev-container)
- [API Reference](#api-reference)
- [Project Structure](#project-structure)
- [Database Schema](#database-schema)
- [Roadmap](#roadmap)
- [AI Usage](#ai-usage)
- [My Thoughts](#my-thoughts)
- [License](#license)

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
- **Manual memory safety** - Without disciplined use of smart pointers or RAII wrappers, dangling pointers and resource leaks are possible. This project manages SQLite connections manually, which requires careful auditing.
- **SQLite access may not be thread-safe** - Each operation opens and closes a fresh SQLite connection, which avoids shared-state races but adds syscall overhead per request. A connection pool would be needed for high throughput.
- **SHA-256 for password hashing** - SHA-256 is a fast cryptographic hash, not a password-hardening function. It was chosen intentionally for simplicity. Production systems should use Argon2 or bcrypt.
- **Steeper onboarding** - Contributors familiar only with JavaScript or Python will need time to get comfortable with the build system, templates, and memory model.
- **Smaller ecosystem** - Fewer ready-made middleware and plug-and-play libraries compared to the Node.js or Python ecosystems.

---

## Features

- **User Registration** - Accepts a username and password; stores the user with a SHA-256 hashed password and a UUID primary key.
- **HTTP Basic Auth Middleware** - All routes except `POST /register` are gated by a Crow middleware that decodes the `Authorization` header, verifies credentials, and injects the authenticated `user_id` into the request context.
- **Current User** - `GET /me` returns the authenticated user's `id` and `username`.
- **Full Todo CRUD** - Create, list, update completion status, and delete todos, all scoped to the authenticated user.
- **SQLite3 Persistence** - Lightweight embedded database with WAL journal mode for improved concurrency.
- **GZIP Compression** - All responses are served with GZIP compression.
- **Layered Architecture** - Clean separation between middleware, controllers, services, repositories, models, and utilities.

---

## Architecture

```mermaid
flowchart TD
    A["HTTP Request"] --> B["AuthMiddleware<br/>Decodes Basic Auth header<br/>Injects user_id into context<br/>Bypasses /register"]
    B --> C["Controllers<br/>auth_controller - POST /register, GET /me<br/>todo_controller - POST /todos, GET /todos"]
    C --> D["Services<br/>AuthService - registerUser, verifyBasicAuth<br/>Hashing - SHA-256 via OpenSSL<br/>UUID - UUID v4 generation"]
    D --> E["Repositories<br/>UserRepository - user lookup & creation<br/>TodoRepository - todo create & list"]
    E --> F["Database<br/>SQLite3 - connection management<br/>Schema initialisation on startup"]
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

## Docker 

- Clone the repo - `git clone --recurse-submodules https://github.com/Vedabahu/todo-app-cpp`
- Build the image - `docker build -f docker/Dockerfile -t todo-app-cpp .`
- Run the container - `docker run -p 18080:18080 todo-app-cpp`

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

### `GET /me`

Returns the authenticated user's profile.

**Response** `200 OK`:
```json
{
  "id": "<uuid>",
  "username": "Vedabahu"
}
```

| Status | Meaning        |
|--------|----------------|
| `200`  | OK             |
| `401`  | Unauthorized   |
| `404`  | User not found |

---

### `POST /todos`

Create a new todo item for the authenticated user.

**Request Body** *(application/json)*:
```json
{
  "title": "Buy groceries"
}
```

**Response** `201 Created`:
```json
{
  "id": "<uuid>",
  "title": "Buy groceries",
  "completed": false
}
```

| Status | Meaning                          |
|--------|----------------------------------|
| `201`  | Todo created                     |
| `400`  | Missing/invalid title or JSON    |
| `401`  | Unauthorized                     |

---

### `GET /todos`

List all todos belonging to the authenticated user.

**Response** `200 OK`:
```json
[
  { "id": "<uuid>", "title": "Buy groceries", "completed": false },
  { "id": "<uuid>", "title": "Write tests",   "completed": false }
]
```

| Status | Meaning      |
|--------|--------------|
| `200`  | OK           |
| `401`  | Unauthorized |

---

### `PATCH /todos/:id`

Update the `completed` status of a todo. Only the owner can update their own todos.

**Request Body** *(application/json)*:
```json
{
  "completed": true
}
```

| Status | Meaning                              |
|--------|--------------------------------------|
| `200`  | Updated                              |
| `400`  | Missing/invalid `completed` field    |
| `401`  | Unauthorized                         |
| `404`  | Todo not found or not owned by user  |

---

### `DELETE /todos/:id`

Delete a todo. Only the owner can delete their own todos.

| Status | Meaning                              |
|--------|--------------------------------------|
| `200`  | Deleted                              |
| `401`  | Unauthorized                         |
| `404`  | Todo not found or not owned by user  |

---

## Project Structure

```
todo-app-cpp/
├── .devcontainer/               # VS Code Dev Container configuration
│   ├── Dockerfile
│   └── devcontainer.json
├── external/
│   └── Crow/                    # Crow HTTP framework (git submodule)
├── src/
│   ├── main.cpp                 # Entry point: wires middleware, DB, repos, services, routes
│   ├── middleware/
│   │   └── auth_middleware.hpp  # Crow middleware: Basic Auth decode & user_id injection
│   ├── controllers/
│   │   ├── auth_controller.hpp  # POST /register, GET /me
│   │   └── todo_controller.hpp  # POST /todos, GET /todos, PATCH /todos/:id, DELETE /todos/:id
│   ├── database/
│   │   ├── database.hpp
│   │   └── database.cpp         # SQLite connection management & schema init
│   ├── models/
│   │   ├── user.hpp             # User struct (id, username, password_hash)
│   │   └── todo.hpp             # Todo struct (id, user_id, title, completed)
│   ├── repositories/
│   │   ├── user_repository.hpp / .cpp   # SQL queries for user lookup & creation
│   │   └── todo_repository.hpp / .cpp   # SQL queries for todo create, list, update & delete
│   ├── services/
│   │   ├── auth_service.hpp
│   │   └── auth_service.cpp     # registerUser(), verifyBasicAuth()
│   └── utils/
│       ├── hashing.hpp / .cpp   # SHA-256 via OpenSSL
│       └── uuid.hpp / .cpp      # UUID v4 generation
├── data/                        # Runtime database files (auto-created, gitignored)
│   └── todo.db
├── .clang-format                # Clang-format style config
├── CMakeLists.txt
├── CMakePresets.json            # Defines the "vcpkg" configure preset
└── vcpkg.json                   # vcpkg manifest with dependency declarations
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
    FOREIGN KEY(user_id) REFERENCES users(id)
);
```

> WAL (Write-Ahead Logging) mode is enabled for better concurrent read/write performance.

---

## Roadmap

- [x] **Basic Auth middleware** - Decodes `Authorization` header, verifies credentials, injects `user_id` into Crow context
- [x] **`POST /todos`** - Create a todo for the authenticated user
- [x] **`GET /todos`** - List all todos for the authenticated user
- [x] **`GET /me`** - Return the authenticated user's profile
- [x] **`PATCH /todos/:id`** - Update the `completed` status of a todo
- [x] **`DELETE /todos/:id`** - Delete a todo
- [X] **Docker** - Containerized deployment
- [ ] **Password hardening** *(optional)* - Swap SHA-256 for Argon2 or bcrypt

---

## AI Usage

This project was built with AI assistance in specific, deliberate ways:

- **Planning & directory structure** - AI was used to think through the layered architecture (middleware → controllers → services → repositories → database).
- **README** - This document was written and iteratively refined with AI assistance based on the actual code.
- **Code & debugging** - All implementation code was written and debugged by me.

I think this is an honest and reasonable way to use AI: for high-level structural decisions and documentation where it saves significant time, while keeping the actual programming work as the learning experience.

---

## My Thoughts

  I thought this would be simple. It was kind of simple but was also hard. Many times, things did not work the way they were supposed to because of some typo or some other silly mistake. For example, I have broken the SQL queries into 2 strings and just placed them side by side. I forgot to add a space and now it does not work properly. All errors were fixable in the end. But overall, it follows the same pattern over and over again, so it was nice. (maybe I should have used something like an ORM?)

  It was a good learning experience. I have learned a lot about C++ and web development. I still like C++.

---

## License

This project is licensed under the **GNU General Public License v3.0**. See [LICENSE](LICENSE) for full terms.
