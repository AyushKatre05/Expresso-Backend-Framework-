use std::collections::HashMap;
use std::io::{self, BufRead, BufReader, Read};
use std::net::TcpStream;

#[derive(Debug)]
pub struct HttpRequest {
    pub method: String,
    pub path: String,
    pub version: String,
    pub headers: HashMap<String, String>,
    pub body: String,
}

impl HttpRequest {
    pub fn parse(stream: &mut TcpStream) -> io::Result<HttpRequest> {
        let mut reader = BufReader::new(stream);
        let mut first_line = String::new();
        
        // 1. Read Request Line
        if let Ok(bytes_read) = reader.read_line(&mut first_line) {
            if bytes_read == 0 {
                return Err(io::Error::new(io::ErrorKind::UnexpectedEof, "Empty request"));
            }
        }

        let parts: Vec<&str> = first_line.trim().split_whitespace().collect();
        if parts.len() < 3 {
            return Err(io::Error::new(io::ErrorKind::InvalidData, "Invalid request line"));
        }

        let method = parts[0].to_string();
        let path = parts[1].to_string();
        let version = parts[2].to_string();

        // 2. Read Headers
        let mut headers = HashMap::new();
        loop {
            let mut line = String::new();
            let bytes_read = reader.read_line(&mut line)?;
            if bytes_read == 0 || line.trim().is_empty() {
                break;
            }
            
            if let Some((key, value)) = line.split_once(':') {
                headers.insert(key.trim().to_string(), value.trim().to_string());
            }
        }

        // 3. Read Body (if Content-Length exists)
        let mut body = String::new();
        if let Some(len_str) = headers.get("Content-Length") {
            if let Ok(len) = len_str.parse::<usize>() {
                let mut buffer = vec![0; len];
                reader.read_exact(&mut buffer)?;
                body = String::from_utf8_lossy(&buffer).to_string();
            }
        }

        Ok(HttpRequest {
            method,
            path,
            version,
            headers,
            body,
        })
    }
}
