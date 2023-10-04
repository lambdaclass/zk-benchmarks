#![no_main]
// If you want to try std support, also update the guest Cargo.toml file
#![no_std]  // std support is experimental

use risc0_zkvm::guest::env;

risc0_zkvm::guest::entry!(main);
pub fn main() {
    let mut i: u32 = 0;

    loop {
        let x = fibonacci(10 * i);
        env::commit(&x);
        i = i + 1;

        if i == 10 {
            break;
        }
    }
}

fn fibonacci(n: u32) -> u64 {
    let mut n = n;
    let mut x0: u64 = 0;
    let mut x1: u64 = 1;
    let mut fib_acc: u64 = 0;

    loop {
        x0 = x1;
        x1 = fib_acc;
        fib_acc = x0 + x1;
        n = n - 1;
        if n == 0 {
            break
        }
    }

    fib_acc
}
