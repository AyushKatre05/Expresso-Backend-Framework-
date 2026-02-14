# Expresso Manager - Quick Reference Guide

## ⚡ TL;DR - Three Quick Starts

### 🏃 LOCAL (Fastest - Development)
```bash
python project_cli.py
1 → cfg
2 → build  
4 → run
# Access: http://localhost:4221
```

### 🐳 DOCKER (Testing)
```bash
python project_cli.py
5 → dock-build
6 → dock-run
# Access: http://localhost:4221
```

### 🚀 COMPOSE + NGINX (Production - Recommended)
```bash
python project_cli.py
7 → comp-up
# Access: http://localhost (port 80 via Nginx)
```

---

## 📊 Quick Command Cheat Sheet

```
LOCAL BUILD:         DOCKER:              COMPOSE:
1 Configure         5 Build Image        7 Compose Up
2 Build             6 Run Container      8 Compose Down
3 Clean             10 Status            9 Logs
4 Run Server        
                    TESTING:
11 Test Endpoints   12 Project Info
13 Custom Cmd       0 Exit
```

---

## 🌐 Quick Endpoints

| Endpoint | Method | Example |
|----------|--------|---------|
| `/` | GET | `curl localhost:4221/` |
| `/echo/{text}` | GET | `curl localhost:4221/echo/hello` |
| `/user-agent` | GET | `curl localhost:4221/user-agent` |
| `/files/{name}` | GET | `curl localhost:4221/files/test.txt` |
| `/files/{name}` | POST | `curl -X POST -d "data" localhost:4221/files/test.txt` |

---

## 🏛️ Architecture Quick View

```
LOCAL:
Client → Expresso (4221)

DOCKER:
Client → Container (4221)

COMPOSE ⭐:
Client → Nginx (80) → Container (4221)
```

---

## 🎯 Quick Decisions

**Which mode should I use?**

| Mode | Use When | Port | Shortcut |
|------|----------|------|----------|
| **Local** | Quick dev/debug | 4221 | `cfg`+`build`+`run` |
| **Docker** | Test containerization | 4221 | `dock-build`+`dock-run` |
| **Compose** | Deploy to production | 80 | `comp-up` |

---

## 🔥 Most Used Commands

```
1️⃣  First Time Setup:        2️⃣  Daily Development:    3️⃣  Production Deploy:
   cmd 1 (cfg)                  cmd 2 (build)             cmd 7 (comp-up)
   cmd 2 (build)                cmd 4 (run)               cmd 11 (test)
   cmd 4 (run)                  cmd 11 (test)             cmd 9 (comp-logs)
   cmd 11 (test)                                          cmd 8 (comp-down)
```

---

## 🛑 Troubleshooting Quick Fix

| Problem | Solution |
|---------|----------|
| Build fails | `3 (clean)` → `1 (cfg)` → `2 (build)` |
| Port in use | Change port or use `13 (cmd)` to kill process |
| Docker error | Install Docker Desktop or use Local mode |
| No response | Run `11 (test)` to diagnose |
| Colors off | Set `FORCE_COLOR=1` and retry |

---

## 📂 Key Files

```
project_cli.py ......... Main manager (ALWAYS RUN THIS)
docker-compose.yml ..... Orchestration config
Dockerfile ............. Image definition
nginx/nginx.conf ....... Reverse proxy config
expresso-server/ ....... Main server code
```

---

## ⌨️ Keyboard Tips

- Type `1-13` or `0` for commands
- Type shortcuts: `cfg`, `build`, `comp-up`, `test`, etc.
- Press `q` to exit anytime
- `Ctrl+C` during server run to stop

---

## 🌐 Access Points

**Local Mode:**
- Direct: `http://localhost:4221`

**Docker Mode:**
- Direct: `http://localhost:4221`

**Compose Mode (Best):**
- Via Nginx: `http://localhost`
- Direct: `http://localhost:4221`

---

Last updated: 2026-02-14 | For full docs see CLI_README.md
