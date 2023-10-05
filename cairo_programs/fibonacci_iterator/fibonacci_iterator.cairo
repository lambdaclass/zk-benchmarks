%builtins range_check

// Looped fibonacci is more efficient
// than calling the fibo function with recursion
// For n = 5, it's 31 steps vs 49 steps
// This is useful to compare with other vms that are not validating the call stack for fibonacci
from starkware.cairo.common.math_cmp import is_le
from starkware.cairo.common.bool import TRUE, FALSE

func main{range_check_ptr: felt}() {
    fib_iter(1, 15);
    // fib_iter2(1,15);
    // fib_iter3(1,15);
    return ();  
}

func fib_mod{range_check_ptr: felt}(first_element: felt, second_element: felt, n) -> (res: felt) {
    alloc_locals;
    jmp fib_body if n != 0;
    tempvar result = second_element;
    return (second_element,);

    fib_body:
    tempvar y = first_element + second_element;
    let comparison = is_le(2147483648, y); 
    if (comparison == TRUE) {
        return fib_mod(second_element, y - 2147483648, n - 1);
    } else {
        return fib_mod(second_element, y, n - 1);
    }
}

func fib_iter{range_check_ptr: felt}(iterator, stop) {
    alloc_locals;
    if (iterator == stop) {
        return ();
    }

    let (res) = fib_mod(0, iterator + 1, 90);
    %{ print("RES: ", ids.res) %}

    fib_iter(iterator + 1, stop);
    return ();
}