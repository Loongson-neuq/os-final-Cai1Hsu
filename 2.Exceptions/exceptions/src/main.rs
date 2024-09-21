use std::arch::asm;

#[no_mangle]
fn stack_overflow() {
    loop {
        unsafe {
            asm!("push rax");
        }
    }
}

#[no_mangle]
fn indrect_null_pointer() {
    unsafe {
        asm!("mov rax, 0");
        asm!("mvq [rax], 1");
    }
}

#[no_mangle]
fn syscall() {
    unsafe {
        asm!("syscall");
    }
}

#[no_mangle]
fn div_by_zero(dividend: f32) -> f32 {
    dividend / 0f32
}

#[no_mangle]
fn invalid_instruction() {
    unsafe {
        // 编译后匹配指令序列，替换成无效指令
        asm!("ud2", "ud2", "syscall", "ud2");
    }
}

fn main() {
    // sp 一直增加，直到访问到栈外保护页，触发操作系统page fault并转化为栈溢出转发至标准库，最终终止进程
    stack_overflow();
    // 写入空指针，触发操作系统page fault并转化为段错误转发至标准库，最终终止进程
    indrect_null_pointer();
    // 触发操作系统trap handler，根据rax的值选择系统调用，处理完成后，返回用户态，并从下一条指令继续执行
    syscall();
    // 触发无效指令异常，操作系统将异常转发至标准库，最终终止进程
    div_by_zero(1.0);
    // 触发无效指令异常，操作系统将异常转发至标准库，最终终止进程
    invalid_instruction();

    println!("You will never see this line!");
}
