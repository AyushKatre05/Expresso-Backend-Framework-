use std::fs::{self, File};
use std::io::{self, Read, Write};
use std::net::TcpStream;
use std::path::Path;
use std::env;

use crate::parser::HttpRequest;

pub fn handle_connection(mut stream: TcpStream) {
    match HttpRequest::parse(&mut stream) {
        Ok(req) => {
            println!("Request: {} {}", req.method, req.path);
            if req.method == "GET" {
                handle_get(stream, &req);
            } else if req.method == "POST" {
                handle_post(stream, &req);
            } else {
                send_response(stream, "405 Method Not Allowed", "text/plain", "Method not allowed");
            }
        },
        Err(e) => {
            eprintln!("Failed to parse request: {}", e);
        }
    }
}

fn send_response(mut stream: TcpStream, status: &str, content_type: &str, body: &str) {
    let response = format!(
        "HTTP/1.1 {}\r\nContent-Type: {}\r\nContent-Length: {}\r\n\r\n{}",
        status, content_type, body.len(), body
    );
    let _ = stream.write_all(response.as_bytes());
}

fn send_bytes(mut stream: TcpStream, status: &str, content_type: &str, body: &[u8]) {
    let headers = format!(
        "HTTP/1.1 {}\r\nContent-Type: {}\r\nContent-Length: {}\r\n\r\n",
        status, content_type, body.len()
    );
    let _ = stream.write_all(headers.as_bytes());
    let _ = stream.write_all(body);
}

fn handle_get(stream: TcpStream, req: &HttpRequest) {
    if req.path == "/" {
        send_response(stream, "200 OK", "text/plain", "Welcome to Expresso (Rust)!");
    } else if req.path.starts_with("/echo/") {
        let content = &req.path[6..];
        send_response(stream, "200 OK", "text/plain", content);
    } else if req.path == "/user-agent" {
        let ua = req.headers.get("User-Agent").map(|s| s.as_str()).unwrap_or("Unknown");
        send_response(stream, "200 OK", "text/plain", ua);
    } else if req.path.starts_with("/files/") {
        let filename = &req.path[7..];
        let dir = env::var("EXPRESSO_DIRECTORY").unwrap_or_else(|_| ".".to_string());
        let filepath = Path::new(&dir).join(filename);
        
        if filepath.exists() {
            if let Ok(contents) = fs::read(filepath) {
                send_bytes(stream, "200 OK", "application/octet-stream", &contents);
            } else {
                send_response(stream, "500 Internal Server Error", "text/plain", "Failed to read file");
            }
        } else {
            send_response(stream, "404 Not Found", "text/plain", "File not found");
        }
    } else {
        // Try serving from docs if configured, or 404
             // For now, simpler handling
        if req.path == "/docs" || req.path == "/docs/" {
             // Mock serving index.html if we were fully implementing static files
             send_response(stream, "200 OK", "text/html", "<h1>Docs</h1>");
             return;
        }

        send_response(stream, "404 Not Found", "text/plain", "Not Found");
    }
}

fn handle_post(stream: TcpStream, req: &HttpRequest) {
    if req.path.starts_with("/files/") {
        let filename = &req.path[7..];
        let dir = env::var("EXPRESSO_DIRECTORY").unwrap_or_else(|_| ".".to_string());
        let _ = fs::create_dir_all(&dir);
        let filepath = Path::new(&dir).join(filename);

        if let Ok(mut file) = File::create(filepath) {
            if file.write_all(req.body.as_bytes()).is_ok() {
                 send_response(stream, "201 Created", "text/plain", "File created");
            } else {
                 send_response(stream, "500 Internal Server Error", "text/plain", "Failed to write file");
            }
        } else {
            send_response(stream, "500 Internal Server Error", "text/plain", "Failed to create file");
        }
    } else {
        send_response(stream, "404 Not Found", "text/plain", "Not Found");
    }
}
