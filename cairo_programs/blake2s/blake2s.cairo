%builtins range_check bitwise

from starkware.cairo.common.alloc import alloc
from starkware.cairo.common.cairo_blake2s.blake2s import blake2s, finalize_blake2s
from starkware.cairo.common.cairo_builtins import BitwiseBuiltin

func main{range_check_ptr, bitwise_ptr: BitwiseBuiltin*}() {
    alloc_locals;

    let inputs_a: felt* = alloc();
    assert inputs_a[0] = 'A';

    let (local blake2s_ptr_start_a) = alloc();
    let blake2s_ptr_a = blake2s_ptr_start_a;
    let (output) = blake2s{range_check_ptr=range_check_ptr, blake2s_ptr=blake2s_ptr_a}(inputs_a, 1);
    assert output.low = 283464815493723968210698558054580609432;
    assert output.high = 32682819570734175424590289175922030236;

    finalize_blake2s(blake2s_ptr_start_a, blake2s_ptr_a);

    let inputs_b: felt* = alloc();
    assert inputs_b[0] = 'B';

    let (local blake2s_ptr_start_b) = alloc();
    let blake2s_ptr_b = blake2s_ptr_start_b;
    let (output) = blake2s{range_check_ptr=range_check_ptr, blake2s_ptr=blake2s_ptr_b}(inputs_b, 1);

    finalize_blake2s(blake2s_ptr_start_b, blake2s_ptr_b);

    let inputs_c: felt* = alloc();
    assert inputs_c[0] = 'C';

    let (local blake2s_ptr_start_c) = alloc();
    let blake2s_ptr_c = blake2s_ptr_start_c;
    let (output) = blake2s{range_check_ptr=range_check_ptr, blake2s_ptr=blake2s_ptr_c}(inputs_c, 1);

    finalize_blake2s(blake2s_ptr_start_c, blake2s_ptr_c);
    return ();
}
