# Benchmarks of proving times of different ZK VMs

To run Stone vs Risc0 benchmarks use

```make bench_stone_risc```

Notice Stone only works in linux

# Additional Data

Security is comparable between VMs. Currently:

- Miden: 96 bits
- Lambdaworks Platinum: 100 bits
- Risc0: 100 bits
- Stone Prover: 100 bits
# Requirements

- Rust
- [cairo-lang (version 0.12.1)](https://github.com/starkware-libs/cairo-lang)
  - pip install cairo-lang==0.12.1
- [risc0 infrastructure](https://github.com/risc0/risc0)
  - cargo install cargo-risczero
  - cargo install risczero 
