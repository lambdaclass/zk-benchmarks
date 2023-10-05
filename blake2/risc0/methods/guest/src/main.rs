#![no_main]
// If you want to try std support, also update the guest Cargo.toml file
#![no_std]  // std support is experimental

use blake2::{Blake2s256, Digest};

use risc0_zkvm::guest::env;

risc0_zkvm::guest::entry!(main);

pub fn main() { // program without loops
    let mut hasher = Blake2s256::new();
    hasher.update(b"A");
    let res = hasher.finalize();

    let arr1 : [u8; 4] = res[0..4].try_into().unwrap();
    let arr2 : [u8; 4] = res[4..8].try_into().unwrap();
    let arr3 : [u8; 4] = res[8..12].try_into().unwrap();
    let arr4 : [u8; 4] = res[12..16].try_into().unwrap();
    let arr5 : [u8; 4] = res[16..20].try_into().unwrap();
    let arr6 : [u8; 4] = res[20..24].try_into().unwrap();
    let arr7 : [u8; 4] = res[24..28].try_into().unwrap();
    let arr8 : [u8; 4] = res[28..32].try_into().unwrap();

    let res : [u32; 8] = [u32::from_be_bytes(arr1), u32::from_be_bytes(arr2), u32::from_be_bytes(arr3), u32::from_be_bytes(arr4), u32::from_be_bytes(arr5), u32::from_be_bytes(arr6), u32::from_be_bytes(arr7), u32::from_be_bytes(arr8)];

    env::commit(&res);

    let mut hasher = Blake2s256::new();
    hasher.update(b"B");
    let _res = hasher.finalize();

    let mut hasher = Blake2s256::new();
    hasher.update(b"C");
    let _res = hasher.finalize();

}
