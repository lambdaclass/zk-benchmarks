SHELL := /bin/bash
CAIRO0_PROGRAMS_DIR=lambdaworks/provers/cairo/cairo_programs/cairo0/benches
CAIRO0_PROGRAMS:=$(wildcard $(CAIRO0_PROGRAMS_DIR)/*.cairo)
COMPILED_CAIRO0_PROGRAMS:=$(patsubst $(CAIRO0_PROGRAMS_DIR)/%.cairo, $(CAIRO0_PROGRAMS_DIR)/%.json, $(CAIRO0_PROGRAMS))

# Rule to compile Cairo programs for testing purposes.
# If the `cairo-lang` toolchain is installed, programs will be compiled with it.
# Otherwise, the cairo_compile docker image will be used
# When using the docker version, be sure to build the image using `make docker_build_cairo_compiler`.
$(CAIRO0_PROGRAMS_DIR)/%.json: $(CAIRO0_PROGRAMS_DIR)/%.cairo
	@echo "Compiling Cairo program..."
	@cairo-compile --cairo_path="$(CAIRO0_PROGRAMS_DIR)" $< --output $@ 2> /dev/null --proof_mode || \
	docker run --rm -v $(ROOT_DIR)/$(CAIRO0_PROGRAMS_DIR):/pwd/$(CAIRO0_PROGRAMS_DIR) cairo --proof_mode /pwd/$< > $@

create_paths:
	echo "{\"trace_path\": \"$(PWD)/cairo_programs/fibonacci_1000/fibonacci_1000_loop_trace.json\",\"memory_path\": \"$(PWD)/cairo_programs/fibonacci_1000/fibonacci_1000_loop_memory.json\"}" > $(PWD)/cairo_programs/fibonacci_1000/fibonacci_1000_looped_private_input.json

fib/risc0/target/release/host:
	cargo build --manifest-path fib/risc0/Cargo.toml --release

time_risc0_fib: fib/risc0/target/release/host
	@time ./fib/risc0/target/release/host

time_stone: create_paths
	time ./stone-prover/cpu_air_prover --out_file=proof.proof --private_input_file=cairo_programs/fibonacci_1000/fibonacci_1000_looped_private_input.json --public_input_file=cairo_programs/fibonacci_1000/fibonacci_1000_looped_public_input.json --parameter_file=cairo_programs/fibonacci_1000/cpu_air_params.json --prover_config_file=stone-prover/e2e_test/cpu_air_prover_config.json

blake2/risc0/target/release/host:
	cargo build --manifest-path blake2/risc0/Cargo.toml --release

time_risc0_blake2: blake2/risc0/target/release/host
	@time ./blake2/risc0/target/release/host

bench: $(COMPILED_CAIRO0_PROGRAMS) risc0/fib/target/release/host
	@echo -e "\n\nMMiden bench parallel\n"
	@(cd miden/benchmarking-cli && cargo run --release -- -e fibonacci) 
	@echo -e "\n\nMiden bench sequential\n"
	@(cd miden/benchmarking-cli && RAYON_NUM_THREADS=1 cargo run --release -- -e fibonacci)
	@echo -e "\n\nRisc Zero bench sequential\n"
	@(time RAYON_NUM_THREADS=1 ./risc0/fib/target/release/host)
	@echo -e "\n\nRisc Zero bench parallel\n"
	@time ./risc0/fib/target/release/host
