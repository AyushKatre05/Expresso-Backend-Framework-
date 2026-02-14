# Expresso Backend Framework - Complete Stack Manager

A comprehensive terminal UI to manage your **full Expresso production stack**:
- 🔨 **Local C++ Build** - Native compilation & execution
- 🐳 **Docker** - Containerized server
- 🚀 **Docker Compose** - Complete orchestration (Nginx + Server)
- ⚙️ **Nginx Reverse Proxy** - Production-grade reverse proxy (port 80 → 4221)

## 🚀 Quick Start

### Windows (Easiest)
```bash
run_cli.bat
```

### PowerShell
```powershell
pwsh -ExecutionPolicy Bypass -File run_cli.ps1
```

### Direct Python
```bash
python project_cli.py
```

## 📦 Project Architecture

Your Expresso framework consists of 4 integrated modules:

| Module | Description | Location |
|--------|-------------|----------|
| **expresso-parser** | HTTP request/response parsing (C) | `expresso-parser/` |
| **expresso-requests** | HTTP request handling (C) | `expresso-requests/` |
| **expresso-types** | Type definitions & structures (C) | `expresso-types/` |
| **expresso-server** | Main HTTP server (C++23, CMake) | `expresso-server/` |

## 🎯 Available Commands (13 Total)

### 🔨 Local Build & Run (Commands 1-4)

| # | Command | Shortcut | Description |
|---|---------|----------|-------------|
| **1** | **Configure CMake** | `cfg` | Setup build environment (C++23, pthreads, zlib) |
| **2** | **Build Server (Local)** | `build` | Compile Expresso server natively |
| **3** | **Clean Build** | `clean` | Remove build artifacts |
| **4** | **Run Server (Local)** | `run` | Start server on port 4221 (local) |

### 🐳 Docker Build (Commands 5-6)

| # | Command | Shortcut | Description |
|---|---------|----------|-------------|
| **5** | **Build Docker Image** | `dock-build` | Build `expresso-server:latest` image |
| **6** | **Build & Run Docker** | `dock-run` | Build image and start container |

### 🚀 Docker Compose Commands (Commands 7-9)

| # | Command | Shortcut | Description |
|---|---------|----------|-------------|
| **7** | **Compose Up** | `comp-up` | Start full stack: Nginx (80) + Server (4221) |
| **8** | **Compose Down** | `comp-down` | Stop all Docker Compose services |
| **9** | **Compose Logs** | `comp-logs` | View real-time logs (Nginx + Server) |

### 📊 Management Commands (Commands 10-13 + 0)

| # | Command | Shortcut | Description |
|---|---------|----------|-------------|
| **10** | **Container Status** | `status` | View running Docker containers |
| **11** | **Test Endpoints** | `test` | Test HTTP endpoints (local, compose, or both) |
| **12** | **Project Info** | `info` | Display full architecture & setup |
| **13** | **Custom Command** | `cmd` | Run arbitrary commands |
| **0** | **Exit** | `q` | Quit the manager |

## 📋 Three Ways to Run Your Server

### 1️⃣ **LOCAL MODE** (Development)
```
CMD 1 → Configure CMake
CMD 2 → Build Server
CMD 4 → Run Server
Endpoint: http://localhost:4221
```
**Best for:** Quick development, debugging, testing locally

---

### 2️⃣ **DOCKER MODE** (Testing)
```
CMD 5 → Build Docker Image
CMD 6 → Build & Run Docker
Endpoint: http://localhost:4221
```
**Best for:** Testing containerization, isolated environment

---

### 3️⃣ **DOCKER COMPOSE + NGINX MODE** ⭐ (Production)
```
CMD 7 → Compose Up
Nginx (port 80) → Routes to server (4221)
Endpoint: http://localhost
```

**Architecture:**
```
Client Request (port 80)
    ↓
Nginx Reverse Proxy
    ↓
Sets Headers (X-Real-IP, X-Forwarded-For, etc.)
    ↓
expresso-server (port 4221)
    ↓
Response through Nginx
    ↓
Client
```

**Best for:** Production deployment, load balancing, SSL termination

---

## 🌐 HTTP Server Endpoints

All three modes support the same endpoints:

### GET Endpoints
- **`GET /`** → Returns `200 OK` (root endpoint)
- **`GET /echo/{text}`** → Echoes text with automatic gzip compression
- **`GET /user-agent`** → Returns `User-Agent` header value
- **`GET /files/{filename}`** → Reads file from `/data` directory

### POST Endpoints
- **`POST /files/{filename}`** → Writes file to `/data` directory (returns `201 Created`)

### Testing Endpoints
Use **Command 11** (Test Endpoints) to automatically test all endpoints:
```
1) Test Local (localhost:4221)
2) Test Nginx (localhost:80)
3) Test Both
```

### Example Requests
```bash
# Using curl (in any mode)
curl http://localhost/                          # Local: 4221, Compose: 80
curl http://localhost/echo/HelloExpresso
curl http://localhost/user-agent
curl http://localhost/files/myfile.txt
curl -X POST -d "Hello" http://localhost/files/test.txt
```

## 📁 Project Structure

```
Expresso-Backend-Framework/
├── expresso-parser/               # Parser module (C)
│   ├── parser.c
│   └── parser.h
├── expresso-requests/             # Requests module (C)
│   ├── ExpressRequests.c
│   └── ExpressRequests.h
├── expresso-types/                # Types module (C)
│   ├── error.h
│   └── ExpressHttp.h
├── expresso-server/               # Main server (C++23)
│   ├── src/
│   │   └── main.cpp
│   ├── build/                     # Created on first build
│   ├── CMakeLists.txt
│   ├── vcpkg.json
│   └── vcpkg-configuration.json
├── nginx/                         # Nginx configuration
│   └── nginx.conf
├── Dockerfile                     # Docker image definition
├── docker-compose.yml             # Docker Compose orchestration
├── .dockerignore
├── CMakeLists.txt                 # Root CMake config
├── cli/                           # CLI manager folder
│   ├── project_cli.py             # This manager script
│   ├── run_cli.bat                # Windows launcher
│   └── README files
└── data/                          # Shared data directory (created on run)
```

## 🏗️ System Requirements

### For Local Build
- **C++ Compiler**: Supporting C++23 (MSVC 2022, GCC 13+, Clang 16+)
- **CMake**: 3.13 or higher
- **Dependencies**: pthreads, zlib (via vcpkg)

### For Docker
- **Docker**: Latest version
- **Docker Compose**: Latest version (included with Docker Desktop)

### Software
- **Python**: 3.7 or higher
- **pip packages**: `requests` (auto-installed)

---

## 🔧 Common Workflows

### Development Workflow
```
1. Cmd 1 → Configure
2. Cmd  2 → Build locally
3. Cmd 4 → Run locally
4. Cmd 11 → Test endpoints
5. Make changes to code
6. Repeat steps 2-4
```

### Docker Testing Workflow
```
1. Cmd 5 →  Build Docker image
2. Cmd 6 → Run in container
3. Cmd 11 → Test endpoints
4. Cmd 8 → Cleanup
```

### Production Deploy Workflow (Recommended)
```
1. Cmd 7 → Compose up (starts Nginx + Server)
2. Cmd 11 → Test through Nginx
3. Cmd 9 → Monitor logs
4. Cmd 8 → Compose down (when done)
```

---

## 🧪 Testing & Validation

### Automated Testing
```
Cmd 11 → Test Endpoints
Choose: 1 (Local), 2 (Compose), or 3 (Both)
```

The tester will:
- ✅ Test `GET /`
- ✅ Test `GET /echo/HelloExpresso`
- ✅ Test `GET /user-agent`
- ✅ Report status codes and responses

### Manual Testing
```bash
# Local mode (port 4221)
curl http://localhost:4221/
curl http://localhost:4221/echo/test

# Via Nginx (port 80)
curl http://localhost/
curl http://localhost/echo/test
```

---

## 🐛 Troubleshooting

### Python Not Found
```bash
pip install requests
python project_cli.py
```

### Docker Not Available
```bash
# Install Docker Desktop from https://docker.com/products/docker-desktop
# Then run:
docker --version
docker compose --version
```

### CMake Not Found
- **Windows**: Download from https://cmake.org/download/
- **macOS**: `brew install cmake`
- **Linux**: `sudo apt-get install cmake`

### Build Fails
```
1. Cmd 3 → Clean
2. Cmd 1 → Configure
3. Cmd 2 → Build
```

### Connection Refused
- **Local**: Check if `Cmd 4` server is running
- **Compose**: Run `Cmd 7` to start services
- **Docker**: Run `Cmd 6` to start container

### Port Already in Use
```bash
# Find what's using the port
lsof -i :4221        # Local
lsof -i :80          # Nginx
docker ps            # See running containers

# Kill process or change port in code
```

---

## 📚 Advanced Usage

### View Config Files
```bash
# Dockerfile
cat Dockerfile

# Nginx reverse proxy config
cat nginx/nginx.conf

# Docker Compose orchestration
cat docker-compose.yml

# Root CMake
cat CMakeLists.txt
```

### Custom Commands (Cmd 13)
```bash
docker ps              # List containers
docker images          # List images
docker compose logs    # View logs
docker compose ps      # Service status
cmake --version        # Check CMake
```

### Build Custom Dockerfile
Edit `Dockerfile` then:
```bash
Cmd 5 → Rebuild image
Cmd 6 → Test new image
```

### Modify Nginx Config
Edit `nginx/nginx.conf` then:
```bash
Cmd 8 → Stop compose
Cmd 7 → Start compose (reloads config)
```

---

## 🎓 Learning Resources

### Understanding the Flow
1. **Local**: Client → Expresso Server
2. **Docker**: Client → Container → Expresso Server
3. **Compose**: Client → Nginx → Container → Expresso Server

### Key Features
- **Request Parsing**: HTTP/1.1 parsing with expresso-parser
- **Gzip Compression**: Automatic with Accept-Encoding
- **Persistent Storage**: `/data` directory shared across modes
- **Multi-threaded**: Thread pool for handling requests
- **Health Checks**: Docker Compose includes health checks

---

## 📝 License

Part of the Expresso Backend Framework project.

## 🤝 Support

For issues:
1. Check this README's Troubleshooting section
2. Use `Cmd 13` to run diagnostic commands
3. Check `Cmd 9` for Linux compose logs
4. Verify requirements are installed

---

**Expresso Project Manager** - Manage your full stack with one command! 🚀
