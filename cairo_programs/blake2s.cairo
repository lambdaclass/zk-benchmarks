%builtins range_check bitwise

from starkware.cairo.common.alloc import alloc
from starkware.cairo.common.cairo_blake2s.blake2s import blake2s, finalize_blake2s
from starkware.cairo.common.cairo_builtins import BitwiseBuiltin

func hash_iterator{range_check_ptr: felt, bitwise_ptr: BitwiseBuiltin*}(iterator: felt, stop: felt) {
    alloc_locals;
    if (iterator == stop) {
        return ();
    }
    let inputs: felt* = alloc();
    assert inputs[0] = 'Hell';
    assert inputs[1] = 'o Wo';
    assert inputs[2] = 'rld';
    let (local blake2s_ptr_start) = alloc();
    let blake2s_ptr = blake2s_ptr_start;
    let (output) = blake2s{range_check_ptr=range_check_ptr, blake2s_ptr=blake2s_ptr}(inputs, 11);
    assert output.low = 322254608667921852837228632274081635440;
    assert output.high = 199935372753399074654713859866415557749;

    finalize_blake2s(blake2s_ptr_start, blake2s_ptr);

    return hash_iterator(iterator = iterator + 1, stop = stop);
}


func main{range_check_ptr, bitwise_ptr: BitwiseBuiltin*}() {

    hash_iterator(iterator = 0, stop = 10);


    return ();
}
