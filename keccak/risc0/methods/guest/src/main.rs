#![no_main]
// If you want to try std support, also update the guest Cargo.toml file
#![no_std]  // std support is experimental


use risc0_zkvm::guest::env;
use sha3::{Digest, Keccak256};

risc0_zkvm::guest::entry!(main);
pub fn main() {
    let mut i = 0;

    loop {
        let mut hasher = Keccak256::new();
        hasher.update(b"hello world");
        let res = hasher.finalize();
    
        env::commit(&res.as_slice());

        i = i + 1;
    
        if i < 10 {
            break;
        }
    }
}
