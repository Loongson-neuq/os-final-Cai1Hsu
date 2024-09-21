use core::asm;

#[repr(C)]
struct Timespec {
    tv_sec: u64,  /* seconds */
    tv_nsec: u64, /* nanoseconds */
}

#[no_mangle]
fn MySleep(seconds: f64) {
    let ts = Timespec {
        tv_sec: seconds as u64,
        tv_nsec: ((seconds - seconds as u64 as f64) * 1_000_000_000.0) as u64 % 1_000_000_000,
    };

    unsafe {
        asm!(
            "mov eax, 0xA2",
            "mov ebx, {0}",
            "xor ecx, ecx",
            "syscall",
            in(reg) &ts,
        );    
    }
}