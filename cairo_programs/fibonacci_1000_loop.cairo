
// Looped fibonacci is more efficient
// than calling the fibo function with recursion
// For n = 5, it's 31 steps vs 49 steps
// This is useful to compare with other vms that are not validating the call stack for fibonacci

func main{}() {
    tempvar x0 = 0;
    tempvar x1 = 1;
    tempvar fib_acc = x0 + x1;
    tempvar n = 1000;
    loop:
        tempvar x0 = x1;
        tempvar x1 = fib_acc;
        tempvar fib_acc = x0 + x1;
        tempvar n = n - 1;
        jmp loop if n != 0;

    assert fib_acc = 222450955505511890955301767713383614666194461405743219770606958667979327682;
    return ();
}
