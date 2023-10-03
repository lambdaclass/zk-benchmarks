#![no_main]
// If you want to try std support, also update the guest Cargo.toml file
#![no_std]  // std support is experimental


use risc0_zkvm::guest::env;

risc0_zkvm::guest::entry!(main);
pub fn main() {
    // Load the first number from the host
    let mut x0: u64 = 0;
    // Load the second number from the host
    let mut x1: u64 = 1;
    let mut fib_acc: u64 = x0 + x1;
    let mut n = 1000;

    loop {
        x0 = x1;
        x1 = fib_acc;
        fib_acc = x0 + x1;
        n = n - 1;
        if n == 0 {
            break
        }
    }

    env::commit(&fib_acc);
}
