#!/usr/bin/env python3
"""
Expresso Backend Framework - Complete Project Manager
Terminal UI for managing the full Expresso stack:
- Local C++ server build & run
- Docker image build & container management
- Docker Compose orchestration (Nginx + Server)
- Health checks & status monitoring
"""

import os
import subprocess
import sys
import platform
import time
from pathlib import Path
from typing import Dict, List, Optional

try:
    import requests
    HAS_REQUESTS = True
except ImportError:
    HAS_REQUESTS = False

# Color codes for terminal output
class Colors:
    HEADER = '\033[95m'
    BLUE = '\033[94m'
    CYAN = '\033[96m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    END = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

class ExpressoCLI:
    def __init__(self):
        self.project_root = Path(__file__).parent.parent
        self.expresso_server_dir = self.project_root / "expresso-server"
        self.build_dir = self.expresso_server_dir / "build"
        self.nginx_dir = self.project_root / "nginx"
        self.os_type = platform.system()
        
        # Server settings
        self.server_process = None
        self.server_port = 4221
        self.server_host = "localhost"
        self.server_directory = str(self.project_root / "data")
        
        # Docker settings
        self.docker_available = self._check_docker()
        self.compose_available = self._check_compose()
        
        # Container names
        self.server_container = "expresso-server"
        self.nginx_container = "expresso-nginx"
        
        self.commands: Dict[str, Dict] = {
            # ===== LOCAL BUILD & RUN =====
            "1": {
                "name": "Configure CMake",
                "description": "Setup build environment (C++23, pthreads, zlib)",
                "command": self._configure,
                "shortcut": "cfg"
            },
            "2": {
                "name": "Build Server (Local)",
                "description": "Compile Expresso server natively",
                "command": self._build,
                "shortcut": "build"
            },
            "3": {
                "name": "Clean Build",
                "description": "Remove build artifacts",
                "command": self._clean,
                "shortcut": "clean"
            },
            "4": {
                "name": "Run Server (Local)",
                "description": "Start server on port 4221 (local)",
                "command": self._run_server,
                "shortcut": "run"
            },
            # ===== DOCKER BUILD =====
            "5": {
                "name": "Build Docker Image",
                "description": "Build expresso-server Docker image",
                "command": self._build_docker,
                "shortcut": "dock-build"
            },
            "6": {
                "name": "Build & Run Docker",
                "description": "Build image and start container",
                "command": self._build_and_run_docker,
                "shortcut": "dock-run"
            },
            # ===== DOCKER COMPOSE =====
            "7": {
                "name": "Compose Up (Nginx + Server)",
                "description": "Start full stack: Nginx →4221→ Expresso Server",
                "command": self._compose_up,
                "shortcut": "comp-up"
            },
            "8": {
                "name": "Compose Down",
                "description": "Stop all containers (Nginx + Server)",
                "command": self._compose_down,
                "shortcut": "comp-down"
            },
            "9": {
                "name": "Compose Logs",
                "description": "View logs from all running services",
                "command": self._compose_logs,
                "shortcut": "comp-logs"
            },
            # ===== MANAGEMENT =====
            "10": {
                "name": "Container Status",
                "description": "Check running Docker containers",
                "command": self._container_status,
                "shortcut": "status"
            },
            "11": {
                "name": "Test Endpoints",
                "description": "Test HTTP endpoints on running server",
                "command": self._test_endpoints,
                "shortcut": "test"
            },
            "12": {
                "name": "Project Info",
                "description": "Display architecture and endpoints",
                "command": self._show_info,
                "shortcut": "info"
            },
            "13": {
                "name": "Custom Command",
                "description": "Run any command in project root",
                "command": self._custom_command,
                "shortcut": "cmd"
            },
            "0": {
                "name": "Exit",
                "description": "Quit Expresso Manager",
                "command": None,
                "shortcut": "q"
            }
        }
    
    def _check_docker(self) -> bool:
        """Check if Docker is available"""
        try:
            subprocess.run(["docker", "--version"], capture_output=True, check=True)
            return True
        except:
            return False
    
    def _check_compose(self) -> bool:
        """Check if Docker Compose is available"""
        try:
            subprocess.run(["docker", "compose", "--version"], capture_output=True, check=True)
            return True
        except:
            return False

    def print_header(self):
        print(f"\n{Colors.BOLD}{Colors.CYAN}{'='*75}")
        print(f"  ╔═══════════════════════════════════════════════════════════════════╗")
        print(f"  ║   EXPRESSO BACKEND FRAMEWORK - Full Stack Manager               ║")
        print(f"  ║   Local Build • Docker • Docker Compose • Nginx                 ║")
        print(f"  ╚═══════════════════════════════════════════════════════════════════╝")
        print(f"{'='*75}{Colors.END}\n")
        
        # Show status
        status_local = "🟢 Active" if self.server_process else "🔴 Stopped"
        print(f"  {Colors.BOLD}LOCAL:{Colors.END} {status_local} (port {self.server_port})")
        
        docker_status = f"{'🟢 Available' if self.docker_available else '🔴 Not Found'}"
        compose_status = f"{'🟢 Available' if self.compose_available else '🔴 Not Found'}"
        print(f"  {Colors.BOLD}DOCKER:{Colors.END} {docker_status} | {Colors.BOLD}COMPOSE:{Colors.END} {compose_status}\n")

    def print_commands(self):
        print(f"{Colors.BOLD}{Colors.YELLOW}{'─'*75}")
        print(f"Available Commands:{Colors.END}\n")
        
        sections = {
            "🔨 LOCAL BUILD & RUN": ["1", "2", "3", "4"],
            "🐳 DOCKER BUILD": ["5", "6"],
            "🚀 DOCKER COMPOSE (Nginx + Server)": ["7", "8", "9"],
            "📊 MANAGEMENT": ["10", "11", "12", "13", "0"]
        }
        
        for section, keys in sections.items():
            print(f"{Colors.BOLD}{Colors.BLUE}{section}{Colors.END}")
            for key in keys:
                if key in self.commands:
                    cmd = self.commands[key]
                    shortcut = f" [{Colors.GREEN}{cmd['shortcut']}{Colors.END}]"
                    print(f"  {Colors.BOLD}{key:>2}{Colors.END}) {cmd['name']:<38} {shortcut}")
                    print(f"      {Colors.CYAN}→ {cmd['description']}{Colors.END}\n")
        
        print(f"{Colors.BOLD}{Colors.YELLOW}{'─'*75}{Colors.END}")

    def get_command_suggestion(self, user_input: str) -> Optional[str]:
        """Provide command suggestions based on user input"""
        user_input = user_input.strip().lower()
        
        # Check for direct number or shortcut
        if user_input in self.commands:
            return user_input
        
        for key, cmd in self.commands.items():
            if cmd['shortcut'].lower().startswith(user_input) or key.startswith(user_input):
                return key
        
        return None

    def run_command(self, input_str: str) -> bool:
        """Execute a command based on user input. Returns False to exit."""
        suggestion = self.get_command_suggestion(input_str)
        
        if not suggestion:
            print(f"{Colors.RED}❌ Invalid command. Please try again.{Colors.END}\n")
            return True
        
        if suggestion == "0":
            return False
        
        cmd_data = self.commands[suggestion]
        if cmd_data["command"]:
            cmd_data["command"]()
        
        return True

    def _run_shell(self, command: str, description: str = "", cwd: Optional[Path] = None) -> bool:
        """Execute a shell command"""
        try:
            if description:
                print(f"\n{Colors.BOLD}{Colors.BLUE}▶ {description}{Colors.END}")
            
            cwd = cwd or self.expresso_server_dir
            print(f"{Colors.CYAN}$ {command}{Colors.END}\n")
            
            result = subprocess.run(
                command,
                shell=True,
                cwd=cwd
            )
            
            if result.returncode == 0:
                print(f"\n{Colors.GREEN}✓ Success!{Colors.END}\n")
                return True
            else:
                print(f"\n{Colors.RED}✗ Command failed with return code {result.returncode}{Colors.END}\n")
                return False
        except Exception as e:
            print(f"{Colors.RED}✗ Error: {str(e)}{Colors.END}\n")
            return False

    def _configure(self):
        """Configure CMake with C++23 and required libraries"""
        self.build_dir.mkdir(exist_ok=True)
        print(f"\n{Colors.BOLD}{Colors.BLUE}▶ Configuring CMake (expresso-server){Colors.END}")
        print(f"{Colors.CYAN}C++23, pthreads, zlib (vcpkg){Colors.END}\n")
        self._run_shell(
            "cmake -B build -DCMAKE_BUILD_TYPE=Debug",
            "CMake Configuration",
            cwd=self.expresso_server_dir
        )

    def _build(self):
        """Build the Expresso server locally"""
        if not self.build_dir.exists():
            print(f"{Colors.YELLOW}⚠ Build files don't exist. Configuring first...{Colors.END}\n")
            self._configure()
        
        return self._run_shell(
            "cmake --build build",
            "Building Expresso Server (Local)",
            cwd=self.expresso_server_dir
        )

    def _clean(self):
        """Clean build directory"""
        try:
            if self.build_dir.exists():
                print(f"\n{Colors.BOLD}{Colors.BLUE}▶ Removing build directory...{Colors.END}")
                import shutil
                shutil.rmtree(self.build_dir)
                print(f"{Colors.GREEN}✓ Build cleaned successfully!{Colors.END}\n")
            else:
                print(f"{Colors.YELLOW}⚠ No build directory to clean.{Colors.END}\n")
        except Exception as e:
            print(f"{Colors.RED}✗ Error cleaning: {str(e)}{Colors.END}\n")

    def _run_server(self):
        """Run the Expresso server locally (port 4221)"""
        if self.server_process is not None:
            print(f"{Colors.YELLOW}⚠ Server is already running locally!{Colors.END}\n")
            return
        
        exe_name = "expresso-server.exe" if self.os_type == "Windows" else "expresso-server"
        exe_path = self.build_dir / exe_name
        
        if not exe_path.exists():
            print(f"{Colors.RED}✗ Executable not found. Please build first (command 2).{Colors.END}\n")
            return
        
        # Create data directory if it doesn't exist
        Path(self.server_directory).mkdir(exist_ok=True)
        
        cmd = f'"{exe_path}" --directory "{self.server_directory}"'
        print(f"\n{Colors.BOLD}{Colors.BLUE}▶ Starting Expresso Server (Local){Colors.END}")
        print(f"{Colors.CYAN}$ {cmd}{Colors.END}\n")
        print(f"{Colors.YELLOW}Server running on http://{self.server_host}:{self.server_port}{Colors.END}")
        print(f"{Colors.YELLOW}Data directory: {self.server_directory}{Colors.END}")
        print(f"{Colors.YELLOW}Press Ctrl+C to stop.{Colors.END}\n")
        
        try:
            self.server_process = subprocess.Popen(
                cmd,
                shell=True,
                cwd=self.expresso_server_dir
            )
            self.server_process.wait()
            self.server_process = None
            print(f"\n{Colors.YELLOW}Server stopped.{Colors.END}\n")
        except KeyboardInterrupt:
            if self.server_process:
                self.server_process.terminate()
                self.server_process = None
            print(f"\n{Colors.YELLOW}Server stopped by user.{Colors.END}\n")

    def _build_docker(self):
        """Build Docker image"""
        if not self.docker_available:
            print(f"{Colors.RED}✗ Docker is not available!{Colors.END}\n")
            return
        
        print(f"\n{Colors.BOLD}{Colors.BLUE}▶ Building Docker image: expresso-server{Colors.END}\n")
        self._run_shell(
            "docker build -t expresso-server:latest .",
            "Docker Build",
            cwd=self.project_root
        )

    def _build_and_run_docker(self):
        """Build Docker image and run container"""
        if not self.docker_available:
            print(f"{Colors.RED}✗ Docker is not available!{Colors.END}\n")
            return
        
        # Build first
        self._build_docker()
        input(f"{Colors.YELLOW}Press Enter to run container...{Colors.END}")
        
        # Stop existing container
        self._run_shell(
            f"docker stop {self.server_container} 2>nul || true",
            "Stopping existing container",
            cwd=self.project_root
        )
        
        # Run new container
        print(f"\n{Colors.BOLD}{Colors.BLUE}▶ Running Docker container{Colors.END}\n")
        cmd = f'docker run -d -p 4221:4221 -v expresso-data:/data --name {self.server_container} expresso-server:latest'
        self._run_shell(cmd, "Starting container", cwd=self.project_root)

    def _compose_up(self):
        """Start Docker Compose stack (Nginx + Server)"""
        if not self.compose_available:
            print(f"{Colors.RED}✗ Docker Compose is not available!{Colors.END}\n")
            return
        
        print(f"\n{Colors.BOLD}{Colors.CYAN}▶ Starting Docker Compose Stack{Colors.END}")
        print(f"{Colors.YELLOW}Components: Nginx (port 80) → Expresso Server (4221){Colors.END}\n")
        
        self._run_shell(
            "docker compose up -d",
            "Compose Up",
            cwd=self.project_root
        )
        
        print(f"{Colors.GREEN}✓ Services started!{Colors.END}")
        print(f"{Colors.CYAN}  Nginx: http://localhost{Colors.END}")
        print(f"{Colors.CYAN}  Server Direct: http://localhost:4221{Colors.END}\n")

    def _compose_down(self):
        """Stop Docker Compose stack"""
        if not self.compose_available:
            print(f"{Colors.RED}✗ Docker Compose is not available!{Colors.END}\n")
            return
        
        print(f"\n{Colors.BOLD}{Colors.BLUE}▶ Stopping Docker Compose Stack{Colors.END}\n")
        self._run_shell(
            "docker compose down",
            "Compose Down",
            cwd=self.project_root
        )

    def _compose_logs(self):
        """View Docker Compose logs"""
        if not self.compose_available:
            print(f"{Colors.RED}✗ Docker Compose is not available!{Colors.END}\n")
            return
        
        print(f"\n{Colors.BOLD}{Colors.BLUE}▶ Docker Compose Logs (Press Ctrl+C to exit){Colors.END}\n")
        self._run_shell(
            "docker compose logs -f",
            "Following logs",
            cwd=self.project_root
        )

    def _container_status(self):
        """Check Docker container status"""
        if not self.docker_available:
            print(f"{Colors.RED}✗ Docker is not available!{Colors.END}\n")
            return
        
        print(f"\n{Colors.BOLD}{Colors.CYAN}Docker Container Status{Colors.END}\n")
        
        # Show all containers
        print(f"{Colors.BOLD}{Colors.YELLOW}Running Containers:{Colors.END}\n")
        self._run_shell(
            "docker ps --format 'table {{.Names}}\\t{{.Status}}\\t{{.Ports}}'",
            "",
            cwd=self.project_root
        )

    def _build_and_run(self):
        """Build and run the server"""
        if self._build():
            print(f"{Colors.GREEN}✓ Build successful!{Colors.END}\n")
            input(f"{Colors.YELLOW}Press Enter to start the server...{Colors.END}")
            self._run_server()

    def _stop_server(self):
        """Stop the running server"""
        if self.server_process is None:
            print(f"{Colors.YELLOW}⚠ No server is currently running.{Colors.END}\n")
        else:
            try:
                self.server_process.terminate()
                self.server_process = None
                print(f"{Colors.GREEN}✓ Server stopped successfully!{Colors.END}\n")
            except Exception as e:
                print(f"{Colors.RED}✗ Error stopping server: {str(e)}{Colors.END}\n")

    def _configure_server(self):
        """Configure server settings"""
        print(f"\n{Colors.BOLD}{Colors.CYAN}═══════════════════════════════════{Colors.END}")
        print(f"{Colors.BOLD}Server Configuration{Colors.END}\n")
        
        print(f"Current Settings:")
        print(f"  Host: {self.server_host}")
        print(f"  Port: {self.server_port}")
        print(f"  Data Dir: {self.server_directory}\n")
        
        print(f"Leave empty to keep current value.\n")
        
        host = input(f"Enter host [{self.server_host}]: ").strip()
        if host:
            self.server_host = host
        
        port = input(f"Enter port [{self.server_port}]: ").strip()
        if port:
            try:
                self.server_port = int(port)
            except ValueError:
                print(f"{Colors.RED}✗ Invalid port number{Colors.END}\n")
        
        directory = input(f"Enter data directory [{self.server_directory}]: ").strip()
        if directory:
            self.server_directory = directory
        
        print(f"\n{Colors.GREEN}✓ Settings updated!{Colors.END}\n")

    def _show_info(self):
        """Show complete project information"""
        print(f"\n{Colors.BOLD}{Colors.CYAN}{'═'*75}{Colors.END}")
        print(f"{Colors.BOLD}Expresso Backend Framework - Full Stack Architecture{Colors.END}\n")
        
        # Modules
        modules = {
            "expresso-parser": "HTTP request/response parsing (C/CMake)",
            "expresso-requests": "HTTP request handling & processing (C/CMake)",
            "expresso-types": "Type definitions & HTTP structures (C/CMake)",
            "expresso-server": "Main HTTP server (C++23, CMake, port 4221)"
        }
        
        print(f"{Colors.BOLD}{Colors.YELLOW}📦 Project Modules:{Colors.END}")
        for module, desc in modules.items():
            print(f"  ✓ {Colors.BOLD}{module}{Colors.END}")
            print(f"    {Colors.CYAN}{desc}{Colors.END}")
        
        # Build & Run Options
        print(f"\n{Colors.BOLD}{Colors.YELLOW}🏗️  Build & Run Options:{Colors.END}")
        print(f"  {Colors.GREEN}1. LOCAL{Colors.END}: Build & run natively")
        print(f"     {Colors.CYAN}$ cmake && cmake --build build{Colors.END}")
        print(f"     {Colors.CYAN}$ ./expresso-server --directory /data{Colors.END}")
        print(f"     {Colors.CYAN}Port: 4221 (direct access){Colors.END}\n")
        
        print(f"  {Colors.GREEN}2. DOCKER{Colors.END}: Build image & run container")
        print(f"     {Colors.CYAN}$ docker build -t expresso-server:latest .{Colors.END}")
        print(f"     {Colors.CYAN}$ docker run -p 4221:4221 expresso-server:latest{Colors.END}")
        print(f"     {Colors.CYAN}Port: 4221 (container){Colors.END}\n")
        
        print(f"  {Colors.GREEN}3. DOCKER COMPOSE{Colors.END} (Recommended for production):")
        print(f"     {Colors.CYAN}Services: Nginx (port 80) + Expresso Server (4221){Colors.END}")
        print(f"     {Colors.CYAN}$ docker compose up -d{Colors.END}")
        print(f"     {Colors.CYAN}Access: http://localhost (via Nginx proxy){Colors.END}\n")
        
        # Server Endpoints
        print(f"{Colors.BOLD}{Colors.YELLOW}🌐 Server Endpoints:{Colors.END}")
        endpoints = {
            "GET /": "Returns 200 OK (root endpoint)",
            "GET /echo/{text}": "Echoes text with gzip compression",
            "GET /user-agent": "Returns User-Agent header",
            "GET /files/{filename}": "Read file from /data directory",
            "POST /files/{filename}": "Write file to /data directory (201 Created)"
        }
        for endpoint, desc in endpoints.items():
            print(f"  {Colors.GREEN}{endpoint}{Colors.END}")
            print(f"    {Colors.CYAN}→ {desc}{Colors.END}")
        
        # Architecture
        print(f"\n{Colors.BOLD}{Colors.YELLOW}🏛️  Architecture - Flow:{Colors.END}")
        print(f"  {Colors.GREEN}Local Mode:{Colors.END}")
        print(f"    Client → Expresso Server (4221)\n")
        
        print(f"  {Colors.GREEN}Docker Mode:{Colors.END}")
        print(f"    Client → Docker Container (4221)\n")
        
        print(f"  {Colors.GREEN}Compose Mode (Recommended):{Colors.END}")
        print(f"    Client\n")
        print(f"      ↓ HTTP (port 80)")
        print(f"    Nginx Reverse Proxy\n")
        print(f"      ↓ Proxy to :4221")
        print(f"    Expresso Server Container\n")
        
        # Project locations
        print(f"{Colors.BOLD}{Colors.YELLOW}📂 Project Locations:{Colors.END}")
        print(f"  Root: {self.project_root}")
        print(f"  Server: {self.expresso_server_dir}")
        print(f"  Build: {self.build_dir}")
        print(f"  Data: {self.server_directory}")
        print(f"  Nginx Config: {self.nginx_dir}\n")

    def _test_endpoints(self):
        """Test API endpoints (Local, Docker, or Compose)"""
        print(f"\n{Colors.BOLD}{Colors.CYAN}🧪 API Endpoint Tester{Colors.END}\n")
        
        if not HAS_REQUESTS:
            print(f"{Colors.YELLOW}⚠ requests library not installed.{Colors.END}")
            print(f"Install with: pip install requests\n")
            return
        
        # Determine which endpoint to test
        endpoints_to_test = []
        print(f"{Colors.BOLD}{Colors.YELLOW}Select endpoint to test:{Colors.END}\n")
        print(f"  1) Local (localhost:4221) - if running locally")
        print(f"  2) Docker Compose (localhost:80) - if using docker compose")
        print(f"  3) Both\n")
        
        choice = input(f"{Colors.BOLD}Choice [1-3]: {Colors.END}").strip()
        
        if choice == "1" or choice == "3":
            endpoints_to_test.append(("Local", "http://localhost:4221"))
        if choice == "2" or choice == "3":
            endpoints_to_test.append(("Compose/Nginx", "http://localhost"))
        
        if not endpoints_to_test:
            return
        
        # Test each endpoint
        for label, base_url in endpoints_to_test:
            print(f"\n{Colors.BOLD}{Colors.CYAN}Testing {label}: {base_url}{Colors.END}\n")
            
            tests = [
                ("GET", "/", "Root endpoint"),
                ("GET", "/echo/Hello%20Expresso", "Echo endpoint"),
                ("GET", "/user-agent", "User-Agent endpoint"),
            ]
            
            for method, endpoint, description in tests:
                try:
                    url = base_url + endpoint
                    print(f"{Colors.BOLD}{method} {endpoint}{Colors.END}")
                    print(f"{Colors.CYAN}→ {description}{Colors.END}")
                    
                    response = requests.get(url, timeout=5)
                    print(f"  Status: {Colors.GREEN}{response.status_code}{Colors.END}")
                    if response.text:
                        text = response.text[:100]
                        print(f"  Response: {text}")
                    print()
                except requests.exceptions.ConnectionError:
                    print(f"  {Colors.RED}✗ Connection refused{Colors.END}")
                    print(f"  {Colors.YELLOW}Make sure server is running on {label}{Colors.END}\n")
                except Exception as e:
                    print(f"  {Colors.RED}✗ Error: {str(e)}{Colors.END}\n")

    def _view_build(self):
        """View build status"""
        print(f"\n{Colors.BOLD}{Colors.CYAN}Build Status{Colors.END}\n")
        
        if self.build_dir.exists():
            print(f"{Colors.GREEN}✓ Build directory exists{Colors.END}")
            exe_name = "http-server.exe" if self.os_type == "Windows" else "http-server"
            exe_path = self.build_dir / exe_name
            
            if exe_path.exists():
                print(f"{Colors.GREEN}✓ Executable found: {exe_name}{Colors.END}\n")
            else:
                print(f"{Colors.RED}✗ Executable not found. Run build command.{Colors.END}\n")
            
            print(f"{Colors.YELLOW}Build directory contents:{Colors.END}")
            try:
                for item in self.build_dir.iterdir():
                    if not item.name.startswith('.'):
                        prefix = "📁" if item.is_dir() else "📄"
                        print(f"  {prefix} {item.name}")
            except:
                pass
            print()
            
            if self.os_type == "Windows":
                response = input(f"{Colors.YELLOW}Open build directory in explorer? (y/n): {Colors.END}").strip().lower()
                if response == 'y':
                    os.startfile(self.build_dir)
        else:
            print(f"{Colors.RED}✗ Build directory doesn't exist. Run configure or build.{Colors.END}\n")

    def _custom_command(self):
        """Run a custom command"""
        print(f"\n{Colors.BOLD}{Colors.YELLOW}Custom Command Executor{Colors.END}\n")
        print(f"{Colors.CYAN}Commands run in: {self.project_root}{Colors.END}")
        print(f"Example: docker ps, cat Dockerfile, etc.\n")
        custom_cmd = input(f"{Colors.BOLD}${Colors.END} ").strip()
        
        if custom_cmd:
            self._run_shell(custom_cmd, "Running custom command", cwd=self.project_root)

    def run(self):
        """Main CLI loop"""
        while True:
            self.print_header()
            self.print_commands()
            
            user_input = input(f"\n{Colors.BOLD}{Colors.GREEN}Select command (number/shortcut):{Colors.END} ").strip()
            
            if not user_input:
                continue
            
            if not self.run_command(user_input):
                print(f"\n{Colors.BOLD}{Colors.CYAN}╔═══════════════════════════════════════════════════════════╗{Colors.END}")
                print(f"{Colors.BOLD}{Colors.CYAN}║  Thank you for using Expresso Project Manager!             ║{Colors.END}")
                print(f"{Colors.BOLD}{Colors.CYAN}║  Happy coding! 🚀 Docker 🐳 Nginx ⚙️  C++23 ⚡            ║{Colors.END}")
                print(f"{Colors.BOLD}{Colors.CYAN}╚═══════════════════════════════════════════════════════════╝{Colors.END}\n")
                break
            
            input(f"{Colors.YELLOW}Press Enter to continue...{Colors.END}")


def main():
    try:
        cli = ExpressoCLI()
        cli.run()
    except KeyboardInterrupt:
        print(f"\n{Colors.YELLOW}CLI interrupted by user.{Colors.END}")
        sys.exit(0)
    except Exception as e:
        print(f"{Colors.RED}Fatal error: {str(e)}{Colors.END}")
        import traceback
        traceback.print_exc()
        sys.exit(1)


if __name__ == "__main__":
    main()
