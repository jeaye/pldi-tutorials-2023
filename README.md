# Overview

# Build instructions

```bash
mkdir build && cd build
cmake -DLLVM_DIR=/path/to/llvm/ -DClang_Dir=/path/to/clang ../
make
./bin/jank
clang-repl> foo<scenario_default>();
foo<default>
clang-repl> foo<scenario_specialized_in_cpp>();
foo<scenario_specialized_in_cpp>
clang-repl> foo<scenario_specialized_inline>();
JIT session error: Symbols not found: [ _Z3fooIL8scenario1EEvv ]
error: Failed to materialize symbols: { (main, { $.incr_module_2.__inits.0, __orc_init_func.incr_module_2 }) }
```
