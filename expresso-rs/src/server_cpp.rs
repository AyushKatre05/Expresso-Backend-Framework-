use std::ffi::{CString, CStr};
use std::os::raw::c_char;

extern "C" {
    // Returns a pointer to a string (C-style)
    fn cpp_process_command(command: *const c_char) -> *const c_char;
}

pub fn log_request_via_cpp(path: &str) {
    process_command_via_cpp(path);
}

pub fn process_command_via_cpp(cmd: &str) -> String {
    let c_cmd = CString::new(cmd).unwrap();
    
    unsafe {
        let ptr = cpp_process_command(c_cmd.as_ptr());
        if !ptr.is_null() {
            let c_str = CStr::from_ptr(ptr);
            c_str.to_string_lossy().into_owned()
        } else {
            "C++ Engine Error".to_string()
        }
    }
}
