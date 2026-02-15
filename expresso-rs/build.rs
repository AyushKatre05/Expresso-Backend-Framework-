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

    // 2. Compile C++ Core Logic (if we want to expose it)
    // For now, let's focus on the C parser integration first to ensure stability,
    // as C++ classes are harder to expose via FFI without a C-shim.
    // We will start by proving the C integration works.
}
