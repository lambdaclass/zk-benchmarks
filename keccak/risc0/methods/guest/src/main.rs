#![no_main]
// If you want to try std support, also update the guest Cargo.toml file
#![no_std]  // std support is experimental


use risc0_zkvm::guest::env;
use sha3::{Digest, Keccak256};

risc0_zkvm::guest::entry!(main);
pub fn main() {
    let mut hasher = Keccak256::new();
    hasher.update(b"hello world");
    let mut res = hasher.finalize();

    // Truncate result to 250 bits to match 
    // the starknet keccak result and make a fair comparison.
    *res.first_mut().unwrap() &= 3;
    env::commit(&res.as_slice());
}
