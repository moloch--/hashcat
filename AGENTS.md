# Repository Guidelines

## Project Structure & Module Organization
- `src/` holds the core C sources; hash-mode implementations live under `src/modules/`.
- `include/` contains public headers and shared types.
- `OpenCL/` stores GPU kernel source files.
- `tools/` provides helper scripts and the test harness (`tools/test.pl`, `tools/test.sh`).
- `docs/` and `BUILD*.md` document build and platform specifics.
- `rules/`, `masks/`, `charsets/`, and `example.*` are data and sample inputs used by the binary.

## Build, Test, and Development Commands
- `make clean && make` builds the project (requires Python >= 3.12; see `BUILD.md`).
- `make install` installs the binary and data files on Linux.
- `make win` cross-builds the Windows binary from Linux.
- `./hashcat --help` runs the local binary and prints usage options.
- Platform-specific instructions live in `BUILD_WSL.md`, `BUILD_Docker.md`, `BUILD_macOS.md`, and others.

## Coding Style & Naming Conventions
- C code targets `gnu99` and uses Allman-style braces.
- Indentation is 2 spaces (tabs only where required, e.g., Makefiles).
- Function and variable names are lower-case.
- Prefer positive conditionals (`if (foo == 0)` over `if (!foo)`), and keep index math aligned (e.g., `array[index + 0]`).
- If you use GNU indent, follow the exact flags listed in `README.md`.

## Testing Guidelines
- The primary test harness is the Perl framework: `tools/test.pl --help` and `tools/test.sh --help`.
- Hash-mode test stubs follow `tools/test_modules/m<hash_mode>.pm` naming.
- Scope tests to the affected hash modes or kernels; a full sweep can be long.

## Commit & Pull Request Guidelines
- Commit messages are short and imperative; history shows both plain summaries and scoped formats like `fix(BUILD.md): ...`.
- PRs must describe motivation and impact, and each PR should address exactly one problem.
- Bug fixes should reference an existing issue; performance changes must include benchmarks and trade-offs.
- Code must be MIT/public domain compatible, and PRs require two board member sign-offs.
