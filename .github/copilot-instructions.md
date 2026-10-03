# Copilot instructions

## Build and run

Each `.cpp` file is a separate program with its own `main`; compile one source file at a time rather than linking the exercises together.

- In VS Code, run the default build task **C/C++: clang++ build active file**. It invokes `/usr/bin/clang++` on the active source file and writes the executable beside that file.
- To build and run one exercise from the repository root, for example:

  ```sh
  /usr/bin/clang++ -g cpp_lab/q10_array_menu.cpp -o /tmp/q10_array_menu
  /tmp/q10_array_menu
  ```

  Replace the source and executable names with the exercise you want to run. Most programs read from standard input; `cpp_lab/q11_letter_count_cmdline.cpp` instead expects text as command-line arguments.
- There is no configured automated test suite or lint command. To check an individual exercise, compile that source and run the resulting program with representative input.

## Repository shape

The root contains standalone string exercises, while `cpp_lab/` contains individually numbered programming exercises. The numbered programs cover basic numeric algorithms, strings, arrays, functions, pointers, macros, dynamic allocation, and menu-driven operations. They do not share a library or common entry point; keep changes scoped to the relevant exercise unless a shared dependency is introduced deliberately.

## Code conventions

- Exercise filenames in `cpp_lab/` use the `qNN_... .cpp` pattern, and many source files identify the exercise with a `// QN:` comment. Keep those identifiers aligned when adding or changing an exercise.
- These are small teaching examples: most use standard input/output, print prompts and results directly, and define any helper functions in the same source file as `main`.
- Follow the representation being taught by the exercise. The collection includes both `std::string` examples and explicit character buffers/pointers (notably the string-menu exercise), as well as arrays and manual dynamic allocation. Avoid replacing those mechanisms with a different abstraction when it would obscure the exercise’s subject.
- Existing examples commonly use `using namespace std;`, `endl`, and simple named helper functions. Match the surrounding file’s style rather than imposing a repository-wide rewrite.
