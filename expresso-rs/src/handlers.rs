use std::fs::{self, File};
use std::io::{self, Read, Write};
use std::net::TcpStream;
use std::path::Path;
use std::env;

use crate::parser::HttpRequest;

pub fn handle_connection(mut stream: TcpStream) {
    // Hybrid Integration Demo:
    // 1. Rust handles the socket.
    // 2. C validates/parsers the request (Simulated FFI call).
    // 3. C++ acknowledges the logic (Simulated FFI call).
    
    // Verify using C parser logic
    crate::parser_c::validate_request_via_c("GET / HTTP/1.1");
    // Verify using C++ logic
    crate::server_cpp::log_request_via_cpp("/");

    match HttpRequest::parse(&mut stream) {
        Ok(req) => {
            // DEEP C++ INTEGRATION:
            // Pass the "command" (path) to the C++ engine logic
            let cpp_msg = crate::server_cpp::process_command_via_cpp(&req.path);

            println!("Request: {} {}", req.method, req.path);
            if req.method == "GET" {
                handle_get(stream, &req, &cpp_msg);
            } else if req.method == "POST" {
                handle_post(stream, &req, &cpp_msg);
            } else if req.method == "DELETE" {
                handle_delete(stream, &req, &cpp_msg);
            } else {
                send_response(stream, "405 Method Not Allowed", "text/plain", "Method not allowed", &cpp_msg);
            }
        },
        Err(e) => {
            eprintln!("Failed to parse request: {}", e);
        }
    }
}

fn send_response(mut stream: TcpStream, status: &str, content_type: &str, body: &str, cpp_msg: &str) {
    let response = format!(
        "HTTP/1.1 {}\r\nContent-Type: {}\r\nContent-Length: {}\r\nX-Expresso-Cpp: {}\r\n\r\n{}",
        status, content_type, body.len(), cpp_msg, body
    );
    let _ = stream.write_all(response.as_bytes());
}

fn send_bytes(mut stream: TcpStream, status: &str, content_type: &str, body: &[u8], cpp_msg: &str) {
    let headers = format!(
        "HTTP/1.1 {}\r\nContent-Type: {}\r\nContent-Length: {}\r\nX-Expresso-Cpp: {}\r\n\r\n",
        status, content_type, body.len(), cpp_msg
    );
    let _ = stream.write_all(headers.as_bytes());
    let _ = stream.write_all(body);
}

fn handle_get(stream: TcpStream, req: &HttpRequest, cpp_msg: &str) {
    // 1. Serve Dashboard on Root
    if req.path == "/" || req.path == "/index.html" {
        serve_static_file(stream, "docs/index.html", "text/html", cpp_msg);
        return;
    }

    // 2. Serve Static Assets (CSS, JS from docs/)
    if req.path == "/styles.css" {
        serve_static_file(stream, "docs/styles.css", "text/css", cpp_msg);
        return;
    }
    if req.path == "/app.js" {
        serve_static_file(stream, "docs/app.js", "application/javascript", cpp_msg);
        return;
    }
    if req.path == "/manual.html" {
        serve_static_file(stream, "docs/manual.html", "text/html", cpp_msg);
        return;
    }

    // 3. API Endpoints
    if req.path.starts_with("/echo/") {
        let content = &req.path[6..];
        send_response(stream, "200 OK", "text/plain", content, cpp_msg);
    } else if req.path == "/user-agent" {
        let ua = req.headers.get("User-Agent").map(|s| s.as_str()).unwrap_or("Unknown");
        send_response(stream, "200 OK", "text/plain", ua, cpp_msg);
    } else if req.path == "/files" || req.path == "/files/" {
        // Simple 'ls' command
        let dir = env::var("EXPRESSO_DIRECTORY").unwrap_or_else(|_| ".".to_string());
        if let Ok(entries) = fs::read_dir(dir) {
            let mut file_list = String::new();
            for entry in entries {
                if let Ok(entry) = entry {
                    if let Ok(name) = entry.file_name().into_string() {
                        file_list.push_str(&name);
                        file_list.push('\n');
                    }
                }
            }
            send_response(stream, "200 OK", "text/plain", &file_list, cpp_msg);
        } else {
            send_response(stream, "500 Internal Server Error", "text/plain", "Failed to list directory", cpp_msg);
        }
    } else if req.path.starts_with("/files/") {
        let filename = &req.path[7..];
        let dir = env::var("EXPRESSO_DIRECTORY").unwrap_or_else(|_| ".".to_string());
        let filepath = Path::new(&dir).join(filename);
        
        if filepath.exists() {
            if let Ok(contents) = fs::read(filepath) {
                send_bytes(stream, "200 OK", "application/octet-stream", &contents, cpp_msg);
            } else {
                send_response(stream, "500 Internal Server Error", "text/plain", "Failed to read file", cpp_msg);
            }
        } else {
            send_response(stream, "404 Not Found", "text/plain", "File not found", cpp_msg);
        }
    } else {
        send_response(stream, "404 Not Found", "text/plain", "Not Found", cpp_msg);
    }
}

fn handle_post(stream: TcpStream, req: &HttpRequest, cpp_msg: &str) {
    if req.path.starts_with("/files/") {
        let filename = &req.path[7..];
        let dir = env::var("EXPRESSO_DIRECTORY").unwrap_or_else(|_| ".".to_string());
        let _ = fs::create_dir_all(&dir);
        let filepath = Path::new(&dir).join(filename);

        if let Ok(mut file) = File::create(filepath) {
            if file.write_all(req.body.as_bytes()).is_ok() {
                 send_response(stream, "201 Created", "text/plain", "File created", cpp_msg);
            } else {
                 send_response(stream, "500 Internal Server Error", "text/plain", "Failed to write file", cpp_msg);
            }
        } else {
            send_response(stream, "500 Internal Server Error", "text/plain", "Failed to create file", cpp_msg);
        }
    } else if req.path.starts_with("/mkdir/") {
        let dirname = &req.path[7..];
        let dir = env::var("EXPRESSO_DIRECTORY").unwrap_or_else(|_| ".".to_string());
        let new_dir_path = Path::new(&dir).join(dirname);

        if fs::create_dir_all(new_dir_path).is_ok() {
            send_response(stream, "201 Created", "text/plain", "Directory created", cpp_msg);
        } else {
            send_response(stream, "500 Internal Server Error", "text/plain", "Failed to create directory", cpp_msg);
        }
    } else {
        send_response(stream, "404 Not Found", "text/plain", "Not Found", cpp_msg);
    }
}

fn handle_delete(stream: TcpStream, req: &HttpRequest, cpp_msg: &str) {
    if req.path.starts_with("/files/") {
        let filename = &req.path[7..];
        let dir = env::var("EXPRESSO_DIRECTORY").unwrap_or_else(|_| ".".to_string());
        let filepath = Path::new(&dir).join(filename);

        if filepath.exists() {
            if filepath.is_dir() {
                if fs::remove_dir_all(filepath).is_ok() {
                    send_response(stream, "200 OK", "text/plain", "Directory deleted", cpp_msg);
                } else {
                    send_response(stream, "500 Internal Server Error", "text/plain", "Failed to delete directory", cpp_msg);
                }
            } else {
                if fs::remove_file(filepath).is_ok() {
                    send_response(stream, "200 OK", "text/plain", "File deleted", cpp_msg);
                } else {
                    send_response(stream, "500 Internal Server Error", "text/plain", "Failed to delete file", cpp_msg);
                }
            }
        } else {
            send_response(stream, "404 Not Found", "text/plain", "File not found", cpp_msg);
        }
    } else {
        send_response(stream, "404 Not Found", "text/plain", "Not Found", cpp_msg);
    }
}

fn serve_static_file(stream: TcpStream, path: &str, content_type: &str, cpp_msg: &str) {
    if let Ok(contents) = fs::read(path) {
        send_bytes(stream, "200 OK", content_type, &contents, cpp_msg);
    } else {
        // Fallback or Error
        eprintln!("Failed to read static file: {}", path);
        send_response(stream, "404 Not Found", "text/plain", "404 - File Not Found", cpp_msg);
    }
}
