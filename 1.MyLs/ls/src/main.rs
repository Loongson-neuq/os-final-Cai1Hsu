use std::fs;
use std::env;

fn main() {
    let cwd = env::current_dir().unwrap();
    println!("DirEntries for '{}':", cwd.display());

    let paths = fs::read_dir(cwd).unwrap();

    for path in paths {
        let path = path.unwrap().path();
        println!("{}", path.display());
    }
}
