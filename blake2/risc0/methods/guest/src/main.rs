#![no_main]
// If you want to try std support, also update the guest Cargo.toml file
#![no_std]  // std support is experimental

use blake2::{Blake2s256, Digest};

use risc0_zkvm::guest::env;

risc0_zkvm::guest::entry!(main);
pub fn main() {
    let mut i = 0;

    loop {
        let mut hasher = Blake2s256::new();
        hasher.update(b"hello world");
        let res = hasher.finalize();

        env::commit(&res.as_slice());

        i = i + 1;

        if i < 10 {
            break;
        }

    }
}
