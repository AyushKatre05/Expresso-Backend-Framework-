fn main() {
    println!("cargo:rerun-if-changed=../expresso-parser");
    println!("cargo:rerun-if-changed=../expresso-server/src");

    // 1. Compile C Parser
    cc::Build::new()
        .file("../expresso-parser/request_parse.c")
        .file("../expresso-parser/request_builder.c")
        .include("../expresso-parser")
        .include("../expresso-types")
        .compile("parser"); // libparser.a

    // 2. Compile C++ Core Logic
    // We compile the bridge.cpp which exposes the C++ logic to Rust
    cc::Build::new()
        .cpp(true) // Enable C++ compilation
        .file("../expresso-server/src/bridge.cpp")
        .compile("server_cpp"); // libserver_cpp.a
}
