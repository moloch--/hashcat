use std::os::raw::{c_char, c_int, c_void};

#[unsafe(no_mangle)]
pub static GENERIC_PLUGIN_OPTIONS: u32 = 0;

#[unsafe(no_mangle)]
pub static GENERIC_PLUGIN_VERSION: u32 = 712;

#[repr(C)]
pub struct generic_global_ctx_t {
    pub quiet: bool,

    pub workc: i32,
    pub workv: *mut *mut c_char,

    pub profile_dir: *mut c_char,
    pub cache_dir: *mut c_char,

    pub error: bool,
    pub error_msg: [c_char; 256],

    pub gbldata: *mut c_void,
}

#[repr(C)]
pub struct generic_thread_ctx_t {
    pub thrdata: *mut c_void,
}

#[unsafe(no_mangle)]
pub extern "C" fn global_init(
    _global_ctx: *mut generic_global_ctx_t,
    _thread_ctx: *mut *mut generic_thread_ctx_t,
    _hashcat_ctx: *mut c_void,
) -> bool {
    true
}

#[unsafe(no_mangle)]
pub extern "C" fn global_term(
    _global_ctx: *mut generic_global_ctx_t,
    _thread_ctx: *mut *mut generic_thread_ctx_t,
    _hashcat_ctx: *mut c_void,
) {
}

#[unsafe(no_mangle)]
pub extern "C" fn global_keyspace(
    _global_ctx: *mut generic_global_ctx_t,
    _thread_ctx: *mut *mut generic_thread_ctx_t,
    _hashcat_ctx: *mut c_void,
) -> u64 {
    0xffff_ffff_ffff_ffff
}

#[unsafe(no_mangle)]
pub extern "C" fn thread_init(
    _global_ctx: *mut generic_global_ctx_t,
    _thread_ctx: *mut generic_thread_ctx_t,
) -> bool {
    unsafe {
        let buf: Box<[u8; 256]> = Box::new([0; 256]);

        (*_thread_ctx).thrdata = Box::into_raw(buf) as *mut c_void;
    }

    true
}

#[unsafe(no_mangle)]
pub extern "C" fn thread_term(
    _global_ctx: *mut generic_global_ctx_t,
    _thread_ctx: *mut generic_thread_ctx_t,
) {
    unsafe {
        let ptr = (*_thread_ctx).thrdata as *mut [u8; 256];

        let _ = Box::from_raw(ptr);

        (*_thread_ctx).thrdata = std::ptr::null_mut();
    }
}

#[unsafe(no_mangle)]
pub extern "C" fn thread_seek(
    _global_ctx: *mut generic_global_ctx_t,
    _thread_ctx: *mut generic_thread_ctx_t,
    _offset: u64,
) -> bool {
    true
}

#[unsafe(no_mangle)]
pub extern "C" fn thread_next(
    _global_ctx: *mut generic_global_ctx_t,
    _thread_ctx: *mut generic_thread_ctx_t,
    out_buf: *mut u8,
) -> c_int {
    unsafe { std::ptr::copy_nonoverlapping(b"Password1".as_ptr(), out_buf, 9) }

    9
}
