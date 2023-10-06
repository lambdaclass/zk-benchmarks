#![no_main]
// If you want to try std support, also update the guest Cargo.toml file
#![no_std]  // std support is experimental

use blake2::{Blake2s256, Digest};

use risc0_zkvm::guest::env;

risc0_zkvm::guest::entry!(main);

pub fn main() { // program without loops
    let mut hasher = Blake2s256::new();
    hasher.update(b"A");
    let res_a = hasher.finalize();

    let arr1 : [u8; 4] = res_a[0..4].try_into().unwrap();
    let arr2 : [u8; 4] = res_a[4..8].try_into().unwrap();
    let arr3 : [u8; 4] = res_a[8..12].try_into().unwrap();
    let arr4 : [u8; 4] = res_a[12..16].try_into().unwrap();
    let arr5 : [u8; 4] = res_a[16..20].try_into().unwrap();
    let arr6 : [u8; 4] = res_a[20..24].try_into().unwrap();
    let arr7 : [u8; 4] = res_a[24..28].try_into().unwrap();
    let arr8 : [u8; 4] = res_a[28..32].try_into().unwrap();

    let res_a : [u32; 8] = [
        u32::from_be_bytes(arr1),
        u32::from_be_bytes(arr2),
        u32::from_be_bytes(arr3), 
        u32::from_be_bytes(arr4), 
        u32::from_be_bytes(arr5), 
        u32::from_be_bytes(arr6), 
        u32::from_be_bytes(arr7), 
        u32::from_be_bytes(arr8),
    ];

    env::commit(&res_a);

    let mut hasher = Blake2s256::new();
    hasher.update(b"B");
    let res_b = hasher.finalize();
    env::commit(&res_b);

    let mut hasher = Blake2s256::new();
    hasher.update(b"C");
    let res_c = hasher.finalize();
    env::commit(&res_c);
}
