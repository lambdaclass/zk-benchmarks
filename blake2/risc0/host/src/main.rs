// TODO: Update the name of the method loaded by the prover. E.g., if the method
// is `multiply`, replace `METHOD_NAME_ELF` with `MULTIPLY_ELF` and replace
// `METHOD_NAME_ID` with `MULTIPLY_ID`
use methods::{METHOD_NAME_ELF, METHOD_NAME_ID};
use risc0_zkvm::{default_prover, ExecutorEnv};

fn main() {
    // First, we construct an executor environment
    let env = ExecutorEnv::builder().build().unwrap();

    // TODO: add guest input to the executor environment using
    // ExecutorEnvBuilder::add_input().
    // To access this method, you'll need to use the alternate construction
    // ExecutorEnv::builder(), which creates an ExecutorEnvBuilder. When you're
    // done adding input, call ExecutorEnvBuilder::build().

    // For example:
    // let env = ExecutorEnv::builder().add_input(&vec).build().unwrap();

    // Obtain the default prover.
    let prover = default_prover();

    // Produce a receipt by proving the specified ELF binary.
    let receipt = prover.prove_elf(env, METHOD_NAME_ELF).unwrap();
    let bytes = receipt.journal;
    dbg!(u32::from_be_bytes(bytes[0..4].try_into().unwrap()));
    dbg!(u32::from_be_bytes(bytes[4..8].try_into().unwrap()));
    dbg!(u32::from_be_bytes(bytes[8..12].try_into().unwrap()));
    dbg!(u32::from_be_bytes(bytes[12..16].try_into().unwrap()));
    dbg!(u32::from_be_bytes(bytes[16..20].try_into().unwrap()));
    dbg!(u32::from_be_bytes(bytes[20..24].try_into().unwrap()));
    dbg!(u32::from_be_bytes(bytes[24..28].try_into().unwrap()));
    dbg!(u32::from_be_bytes(bytes[28..32].try_into().unwrap()));

    // TODO: Implement code for transmitting or serializing the receipt for
    // other parties to verify here

    // Optional: Verify receipt to confirm that recipients will also be able to
    // verify your receipt
    // receipt.verify(METHOD_NAME_ID).unwrap();
}
