%builtins range_check bitwise

from starkware.cairo.common.cairo_keccak.keccak import cairo_keccak, finalize_keccak
from starkware.cairo.common.uint256 import Uint256
from starkware.cairo.common.cairo_builtins import BitwiseBuiltin
from starkware.cairo.common.alloc import alloc


func hash_iterator{range_check_ptr: felt, bitwise_ptr: BitwiseBuiltin*}(iterator: felt, stop: felt) {
    alloc_locals;
    if (iterator == stop) {
        return ();
    }

    let (keccak_ptr: felt*) = alloc();
    let keccak_ptr_start = keccak_ptr;

    let (inputs: felt*) = alloc();

//   8031924123371070792 == int.from_bytes(b'Hello wo', 'little')
//   560229490 == int.from_bytes(b'rld!', 'little')
    assert inputs[0] = 8031924123371070792;
    assert inputs[1] = 560229490;

    let n_bytes = 16;

    let (res: Uint256) = cairo_keccak{keccak_ptr=keccak_ptr}(inputs=inputs, n_bytes=n_bytes);

    assert res.low = 293431514620200399776069983710520819074;
    assert res.high = 317109767021952548743448767588473366791;

    finalize_keccak(keccak_ptr_start=keccak_ptr_start, keccak_ptr_end=keccak_ptr);


    return hash_iterator(iterator = iterator + 1, stop = stop);
}

func main{range_check_ptr: felt, bitwise_ptr: BitwiseBuiltin*}() {

    hash_iterator(iterator = 0, stop = 10);

    return ();
}
