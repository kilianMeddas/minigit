# MiniGit

MiniGit is an educational **C99** implementation of selected Git internals, developed as part of my studies at **EPITA**. It can initialize a local repository and add files to an index, storing their contents as objects identified by SHA-1 hashes.

## Table of Contents

- [Features](#features)
- [Implementation Roadmap](#implementation-roadmap)
  - [Implemented](#implemented)
  - [Planned](#planned)
- [Requirements](#requirements)
- [Building](#building)
- [Usage](#usage)
- [Architecture](#architecture)
- [Data Storage](#data-storage)
  - [Blob Objects](#blob-objects)
  - [Index](#index)
- [Tests and Maintenance](#tests-and-maintenance)
- [Current Scope and Limitations](#current-scope-and-limitations)

## Features

| Command | Description |
| --- | --- |
| `init` | Creates the `.mgit` repository structure and its `HEAD` file. |
| `add <file> [files…]` | Validates files, writes blob objects, and creates or extends the index. |

Index entries are sorted by path. SHA-1 hashes are computed using OpenSSL.

## Implementation Roadmap

### Implemented

- [x] **`init`** — Initialize a local repository.
- [x] **`add`** — Store files as blob objects and add them to the index.

### Planned

- [ ] **`status`** — Show untracked files and changes relative to the index.
- [ ] **`commit`** — Save a snapshot of the staged files.

## Requirements

- A Linux / POSIX environment.
- GCC, GNU Make, and `pkg-config`.
- OpenSSL libraries and development headers.
- Criterion to run the tests.
- `clang-format` for the optional formatting target.

## Building

From the GitHub repository root:

```sh
git clone https://github.com/kilianMeddas/minigit.git
cd minigit/minigit
make all
```

The executable is generated in the directory containing the Makefile.

> Use `make all` explicitly: the current Makefile's first target is `format`, so running `make` alone formats the source files.

## Usage

From the build directory:

```sh
mkdir demo
cd demo

../minigit init
printf 'Hello MiniGit!\n' > hello.txt
printf 'A second note.\n' > notes.txt
../minigit add hello.txt notes.txt
```

Commands operate in the current directory. Run `init` and then `add` from the root of the working directory containing the relevant `.mgit` repository.

The repository uses the following layout:

| Path | Purpose |
| --- | --- |
| `.mgit/HEAD` | Contains the reference `ref: refs/heads/master`. |
| `.mgit/objects/` | Stores added objects. |
| `.mgit/refs/heads/` | Reserved for local references. |
| `.mgit/refs/remotes/` | Reserved for remote references. |
| `.mgit/index` | Created when files are added. |

The reference directories are created during initialization; branch and remote commands are not implemented.

## Architecture

Paths below are relative to the GitHub repository root.

| File or directory | Responsibility |
| --- | --- |
| `minigit/Makefile` | Building, testing, cleaning, and formatting. |
| `minigit/src/main.c` | Parses arguments and dispatches the `init` and `add` commands. |
| `minigit/src/minigit.h` | Public command interface. |
| `minigit/src/init.c` | Creates the `.mgit` directory structure and `HEAD`. |
| `minigit/src/add.c` | Validates paths, writes objects, and invokes index management. |
| `minigit/src/helpers.c` and `helpers.h` | File reading, blob construction, SHA-1 hashing, and the index entry structure. |
| `minigit/src/index.c` and `index.h` | Reads, sorts, and serializes index entries; computes the index checksum. |
| `minigit/tests/test_suite.c` | Criterion unit tests. |
| `minigit/tests/example_repositories/` | Example repositories at different stages. |
| `minigit/tests/pretty-printer/` | Source code for an index and tree inspection tool. |
| `minigit/printer` | Inspection tool binary included in the repository. |

## Data Storage

### Blob Objects

For each added file, MiniGit constructs the following byte sequence:

```text
blob <size>\0<content>
```

The SHA-1 hash of this sequence determines the storage path. The first two hexadecimal characters form the directory name, and the remaining 38 form the filename.

```text
.mgit/objects/<first 2 characters>/<remaining 38 characters>
```

Objects are stored **without compression**.

### Index

The index uses a project-specific format:

- A 16-byte header containing the `DIRC` signature, version `2`, and a 64-bit entry count.
- Entries containing `struct stat` metadata, a hexadecimal SHA-1 hash, the path length, and the path.
- A final SHA-1 checksum stored as 40 hexadecimal characters.

This format depends on native platform types and **is not compatible with Git's binary index format**, despite using the same signature and version number.

## Tests and Maintenance

From `minigit/`, the directory containing the Makefile:

```sh
make check    # Build the program and run the Criterion tests
make clean    # Remove generated files specified by the Makefile
make format   # Format source files using clang-format
```

The suite contains four tests: repository creation, rejection of repeated initialization, addition of an existing file, and rejection of a missing file. Each test uses a temporary directory.

The inspection tool can be rebuilt separately:

```sh
make -C tests/pretty-printer
tests/pretty-printer/printer -i demo/.mgit/index
```

Whether it can read a given index depends on the format expected by the tool.

## Current Scope and Limitations

- Only `init` and `add` are implemented. `commit`, `status`, branch management, and remote operations are outside the current scope.
- `add` accepts explicit paths to regular files. Directories and symbolic links are rejected, and recursive staging through `add .` is not supported.
- Adding an already indexed path appends another entry instead of replacing the existing one.
- Blob construction uses `strlen`, so binary files containing null bytes are not handled correctly.
- Some index operation errors are not propagated to `add`, so its exit status does not guarantee that every write succeeded.
- Unknown commands currently return `0` without performing any action.

This project explores content-addressed storage, file handling, binary serialization, and memory management in C.
