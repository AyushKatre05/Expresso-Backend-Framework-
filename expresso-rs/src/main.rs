use std::io::prelude::*;
use std::net::{TcpListener, TcpStream};
use std::fs;
use std::thread;
use std::env;

mod parser;
mod parser_c;
mod server_cpp;
mod handlers;
// mod threadpool; // Using the crate directly

fn main() {
    let port_str = env::var("PORT").unwrap_or("4221".to_string());
    let port: u16 = port_str.parse().expect("PORT must be a number");
    
    // Bind to 0.0.0.0 for Render deployment
    let listener = TcpListener::bind(format!("0.0.0.0:{}", port)).unwrap();
    println!("Expresso (Rust) running on port {}", port);

    let pool = threadpool::ThreadPool::new(4);

    for stream in listener.incoming() {
        let stream = stream.unwrap();

        pool.execute(|| {
            handlers::handle_connection(stream);
        });
    }
}
