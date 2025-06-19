# Semestral Work

A Mila (Pascal-like, expression-driven language) compiler with LLVM IR backend.

# Project Structure

- `CMakeLists.txt` - Top-level CMake build configuration
- `CMakeModules/` - CMake helper modules (e.g., for testing)
- `external/` - External dependencies (LLVM 19.1)
- `mila-compiler/` - Main compiler source code
  - `ast/` - Abstract Syntax Tree (AST) definitions and visitors
  - `codegen/` - LLVM IR code generation logic
  - `exception/` - Exception and error handling
  - `lexer/` - Lexical analysis (tokenizer)
  - `parser/` - Parsing logic (syntax analysis)
  - `main.cpp` - Compiler entry point
- `mila` - Wrapper script to build and run the compiler, and link output
- `samples/` - Example Mila source files
- `tests/` - Input/output test cases for sample programs

 The AST is designed using the Visitor pattern, making it easy to extend with new operations such as pretty printers, AST printers, or other analyses without modifying the AST node classes themselves.

Compilation tests and output validation for sample files are implemented using `ctest` and defined in CMake modules.

## Dependencies

### LLVM version

Recommended version: 19.1
A submodule for LLVM 19.1 is provided, and you can build from the submodule if desired.
To build your own LLVM, enable `BUILD_LLVM=ON`.

## Building

This project uses the standard CMake toolchain. If no `CMAKE_BUILD_TYPE` is set, it will automatically build in release mode.

```sh
cmake -Bbuild
cd build
cmake --build .
```

The built compiler outputs LLVM IR, which can be further compiled to a binary.

## MacOS Specific Notes

Install dependencies via Homebrew:
```sh
brew install git
brew install cmake
brew install llvm@19
brew install gnu-getopt
```

Then update your shell rc file (e.g., `.bashrc`, `.zshrc`):
```sh
export PATH="/usr/local/opt/gnu-getopt/bin:$PATH"
```

## Running

The `mila` script in the project root is a wrapper that builds and runs the compiler, then uses `llc` and `clang` to produce an executable. Example usage:

```sh
./mila <source.mila> -o <output_binary>
```

This will:
1. Run the compiler to generate LLVM IR (`.ir` file)
2. Use `llc` to generate assembly (`.s` file)
3. Use `clang` to produce the final executable

## Testing

From inside the `build` directory, you can use the `ctest` command to run all tests.
Tests are defined to:
- Compile all example source codes in `samples/`
- Run the resulting executables and compare their output with expected output in `tests/`

CTest is configured via `CMakeModules/tests.cmake` and uses `CMakeModules/run_test.cmake` to automate input/output checking.

## Compiling a program manually

First, create a source file:
```sh
touch test.mila
```

Then compile it:
```sh
./mila test.mila -o test.out
```

---
