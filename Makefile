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

bench: $(COMPILED_CAIRO0_PROGRAMS)
	(cd miden/benchmarking-cli && cargo run --release -- -e fibonacci) 
