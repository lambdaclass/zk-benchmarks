# Benchmarks of proving times of different ZK VMs

To run Stone vs Risc0 benchmarks, first make sure to activate your Cairo Python environment:

```
source <cairo_venv_path>/bin/activate
```

and then run

```
make bench_stone_risc
```

**Note**: The Stone prover only works in linux.

# Additional Data

Security is comparable between VMs. Currently:

- Miden: 96 bits
- Lambdaworks Platinum: 100 bits
- Risc0: 100 bits
- Stone Prover: 100 bits
# Requirements

* Rust
* [cairo-lang (version 0.12.1)](https://github.com/starkware-libs/cairo-lang):
 It is recommended to create a Python virtual environment to install Cairo dependencies. If you have `pyenv` Python versions manager, you can run `make deps` for Linux or `make deps-macos` for MacOS. This will create a `cairo_venv` Python virtual environment with all the dependencies installed. To do it manually, you can run the following instructions:
  ```
  python3.9 -m venv cairo_venv
  source cairo_venv/bin/activate
  pip3.9 install -r requirements.txt
- [risc0 infrastructure](https://github.com/risc0/risc0)
  - `cargo install cargo-risczero`
  - `cargo risczero install`
