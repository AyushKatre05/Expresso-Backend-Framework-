use std::ffi::CString;
use std::os::raw::c_char;

extern "C" {
    // Real FFI function defined in expresso-server/src/bridge.cpp
    fn cpp_process_command(command: *const c_char) -> i32;
}

pub fn log_request_via_cpp(path: &str) {
    // Re-use the process command for logging
    process_command_via_cpp(path);
}

pub fn process_command_via_cpp(cmd: &str) -> bool {
    let c_cmd = CString::new(cmd).unwrap();
    println!("[RUST] Handing off to C++...");
    
    unsafe {
        let status = cpp_process_command(c_cmd.as_ptr());
        println!("[RUST] C++ Engine returned status: {}", status);
        status == 1 || status == 999
    }
}
