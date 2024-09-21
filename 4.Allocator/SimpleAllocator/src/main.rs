use core::slice;

static mut HEAP: [u8; 4096] = [0; 4096];

static mut ALLOC_RECORDS: Vec<AllocRecord> = Vec::new();

struct AllocRecord {
    offset: usize,
    size: usize,
}

fn is_taken(offset: usize) -> bool {
    unsafe {
        ALLOC_RECORDS.iter().any(|record| {
            let start = record.offset;
            let end = record.offset + record.size;

            offset >= start && offset < end
        })
    }
}

#[no_mangle]
fn alloc(size: usize) -> *mut u8 {
    // TODO: There is something you can improve here
    //    - 8 bytes align to improve performance
    //    - Concurrent access to the heap
    //    - Use a better algorithm to find a free block

    let heap_start = unsafe { HEAP.as_ptr() } as usize;
    let heap_size = unsafe { HEAP.len() };

    let mut offset = 0;
    let mut found = false;

    // find a free block
    let mut i = 0;
    while i < heap_size {
        for j in 0..size {
            if is_taken(i + j) {
                i += j;
                break;
            }

            if j == size - 1 {
                offset = i;
                found = true;
            }
        }
    }

    if !found {
        return std::ptr::null_mut();
    }

    // clear memory
    unsafe { slice::from_raw_parts_mut((heap_start + offset) as *mut u8, size).fill(0) };

    let record = AllocRecord { offset, size };

    // add record
    unsafe {
        ALLOC_RECORDS.push(record);
    }

    (heap_start + offset) as *mut u8
}

#[no_mangle]
fn dealloc(ptr: *mut u8) -> bool {
    let heap_start = unsafe { HEAP.as_ptr() } as usize;

    let idx = unsafe {
        ALLOC_RECORDS.iter().enumerate().find(|(_, record)| {
            let start = record.offset;
            let end = record.offset + record.size;

            let ptr_offset = ptr as usize - heap_start;

            ptr_offset >= start && ptr_offset < end
        })
    };

    match idx {
        Some((index, _)) => {
            unsafe {
                ALLOC_RECORDS.swap_remove(index);
            }

            true
        }
        None => false,
    }
}

fn main() {
    println!("Hello, world!");
}
