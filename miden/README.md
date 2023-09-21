# Miden Assembly Playground

Playground for example programs for the [Miden VM](https://github.com/0xPolygonMiden/miden-vm) in Miden assembly.

[Use the playground!](https://0xpolygonmiden.github.io/examples/)

The goal of this playground is for developers to see how easy it is to write and execute code in Miden assembly. The examples come from the community and the team. If you come up with an example of your own, we are happy to include it in the list below. You can simply open a PR with your example adding the `.masm` and `.inputs` files.

Examples are written in Miden assembly, see [Miden assembly documentation](https://wiki.polygon.technology/docs/miden/user_docs/assembly/main/). External inputs can be provided to the examples and the Miden VM in two ways as public inputs and via the advice provider, see [here](https://wiki.polygon.technology/docs/miden/intro/overview/#inputs-and-outputs). Currently, in the playground you can use `operand_stack` as public input and the `advice_stack` as secret input. 

In addition to running existing examples you can also write your own program and execute it. The examples can then serve as inspiration. 

---
## Available examples
There are several examples in our repo and we hope we get more in the future. The examples range from simple to complex and aim to show how Miden assembly can be used.

### Simple examples - to see how the code works

- **Comparison**: A program which checks if the value provided as secret input via the advice tape is less than 9; if it is, the value is multiplied by 9, otherwise, 9 is added to the value; then we check if the value is odd.

- **Conditional**: A program which either adds or multiplies two numbers - 3 and 5 - based on the value provided via the advice tape as secret input.

### Math examples - see more complex numerical calculations 

- **Collatz**: A program which executes an unbounded loop to compute a Collatz sequence which starts with the provided value; the output of the program is the number of steps needed to reach 1 - the end of the sequence.

- **Fibonnacci**: Elegant way to calculate the 1001st fibonacci number. 

- **nPrime**: Program to calculate the n-th prime number. It will return all prime numbers up to the n-th prime number which is on top of the stack. 

### Complex examples - larger complex nested operations

- **Game-of-Life**: Implementation of Conway's Game of Life. But we can prove it. The static example runs on a 4x4 cell universe and 1000 generations.

---
## Running the playground locally

You can also run the playground locally on your machine. 

### Starting site

If you want to run it locally, then make sure you have the [wasm-pack](https://rustwasm.github.io/wasm-pack/installer/) installed.

To run the playground locally in development mode, navigate to the project directory `playground` and run:

```
npm run build:miden && npm install && npm run start
```

Open [http://localhost:3000](http://localhost:3000) to view it in the browser.

Test rust code:

```
cd miden-wasm && wasm-pack test --node
```

### Acknowledgement 

We use a fork of https://github.com/timgestson/miden-assembly-playground
