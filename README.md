# 🌌 Expresso OS: The Hybrid Backend Framework

**Expresso OS** is a high-fidelity, high-performance web server framework built from first principles using a **Triple-Language Hybrid Architecture** (Rust + C++ + C). 

Unlike traditional "website servers," Expresso is a systems-programming masterpiece that exposes a full-scale **Linux-like Hacker Terminal** as its primary interface, allowing for deep remote administration and logic auditing in real-time.

---

## 🏛️ Architecture: The Hybrid Triple-Threat

Expresso leverages the unique strengths of three industry-standard languages:

1.  **🦀 Rust (The Kernel):** Manages high-performance networking, thread-pooling, and the Virtual File System (VFS).
2.  **💎 C++ (The Logic Engine):** Handles the "High Brain" business logic and real-time security auditing via FFI (Foreign Function Interface).
3.  **🔌 C (The Parser):** Provides ultra-low-latency request parsing for maximum throughput.

---

## 🕶️ The Hacker Terminal UI

Expresso replaces static dashboards with an interactive commands-based interface.
*   **50+ Commands:** A comprehensive suite of Linux-like utilities including `ls`, `cat`, `nano`, `rm`, `mkdir`, `top`, `ps`, and more.
*   **Real-Time Auditing:** Every command triggers a blue `[SYSTEM]` audit log directly from the C++ Engine.
*   **Virtual Navigator:** Support for `cd`, relative paths, and shell-state management.
*   **Embedded Nano:** A fully functional in-browser text editor for real-time file modification.

---

## 🚀 Getting Started

### 1. Run via Docker (Recommended)
The project is containerized for instant deployment.

```bash
# Build and Launch
docker compose up --build
```
Once launched, open **http://localhost:10000** to enter the Expresso Terminal.

### 2. Basic Commands
*   `help` - List all available commands.
*   `man` - Open the architectural manual in a new tab.
*   `status` - Run a deep health check on the Hybrid engine.
*   `matrix` - Initiate the digital rain hacker visual.

---

## 📖 Endpoints (Legacy Integration)

Expresso still functions as a standard HTTP server for external apps:

| Method | Path | Description |
| :--- | :--- | :--- |
| **GET** | `/` | Serves the Hacker Terminal |
| **GET** | `/files` | Lists directory contents (Secure Audit) |
| **POST** | `/files/<name>` | Writes raw body to a file |
| **DELETE** | `/files/<name>` | Deletes a file or directory |
| **GET** | `/echo/<text>` | Low-latency response check |

---

## 📂 Project Structure
*   `expresso-rs/`: The Rust Kernel and FFI bridge.
*   `expresso-server/`: The C++ Logic Engine.
*   `expresso-parser/`: The C Request/Response parser.
*   `docs/`: The frontend Terminal UI and Manual.

---

## 📜 License
Distributed under the MIT License. Built with ❤️ for Systems Engineering.
