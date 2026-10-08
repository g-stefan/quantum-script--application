# Quantum Script Extension Application — Documentation

`quantum-script--application` is the **command line extension of Quantum
Script**. Loaded with `Script.requireExtension("Application")`, it gives a
script access to the command line of the process that runs it and to the
location of that process's executable:

- **Raw command line.** `Application.getCmdN()` / `Application.getCmdS(k)`
  return the `argc` / `argv` the host program received, unchanged (index 0 is
  the executable as it was typed).
- **Script arguments.** `Application.arguments` is an array with every
  argument after the executable, built once when the extension is loaded.
- **Option parsing helpers.** `Application.hasFlag(name)` tests for
  `--name` / `--name=value`, `Application.getFlagValue(name, default)` reads
  `--name=value`, `Application.getArgument(position, default)` reads the
  positional arguments (the ones that do not start with `--`).
- **Where the executable is.** `Application.getPathExecutable()` is the folder
  of the running executable (resolved, absolute), `Application.getExecutable()`
  that folder plus the executable file name. Use it to find files installed
  next to the tool (`fabricare` finds its platform scripts this way).

```
scripts: quantum-script tools, .cmd launchers, fabricare build scripts, ...
quantum-script--application   <-- this extension: Application.*
quantum-script                (Executive: mainCmdN, mainCmdS, pathExecutable)
xyo-system                    (Shell::getExecutablePath, Shell::getFileName)
xyo-encoding, xyo-multithreading, xyo-data-structures, xyo-managed-memory, xyo-platform
```

## Why it exists

Quantum Script has no `process.argv`: the language core (`Script`, `Console`,
...) knows nothing about the process it runs in. The engine keeps the
command line it was started with (`Executive::mainCmdN`, `mainCmdS`) and the
executable folder (`Executive::pathExecutable`); this extension exposes them
to scripts and adds the small amount of option parsing every command line
tool needs.

| Need | What `Application` gives |
|------|--------------------------|
| Write a command line tool in Quantum Script | `getArgument(k)` for file names, `hasFlag` / `getFlagValue` for `--options` |
| A `.cmd` launcher that forwards its arguments | `%*` ends up in `Application.arguments` |
| Find data or scripts installed next to the executable | `getPathExecutable()` |
| Start the same executable again (sub-process, other platform) | `getExecutable()` |
| Look at the exact command line | `getCmdN()`, `getCmdS(k)` |

## Concepts at a glance

| Need | Use | Notes |
|------|-----|-------|
| Load the extension | `Script.requireExtension("Application");` | once per script; `fabricare` has it built in |
| Argument count / value | `Application.getCmdN()`, `Application.getCmdS(k)` | index 0 = executable; `k >= getCmdN()` → `undefined` |
| All arguments after the executable | `Application.arguments` | Array of strings, may be changed by the script |
| Is `--verbose` present? | `Application.hasFlag("verbose")` | also true for `--verbose=...` |
| Value of `--out=file` | `Application.getFlagValue("out", "default.txt")` | only the `--name=value` form; everything after the first `=` |
| k-th positional argument | `Application.getArgument(k, default)` | skips every argument that starts with `--` |
| Executable folder | `Application.getPathExecutable()` | absolute, no trailing separator |
| Executable path | `Application.getExecutable()` | folder + file name as typed in `argv[0]` |

**With the `quantum-script` interpreter the script file is an argument too:**
`quantum-script tool.js input.txt` gives `getArgument(0) == "tool.js"` and
`getArgument(1) == "input.txt"`. See
[Command line model](command-line.md).

## Contents

| Document | What it covers |
|----------|----------------|
| [Getting started](getting-started.md) | Build and install, first script, `.cmd` launchers, fabricare scripts, register it in a C++ host, static builds, threads |
| [Command line model](command-line.md) | What `argv` contains under `quantum-script`, `.cmd` launchers, `--run`, `fabricare` and your own host; flags vs positional arguments; a complete tool; pitfalls |
| [Script API](script-api.md) | Every function and property: arguments, exact behavior, edge cases, return values |
| [API reference](reference.md) | Every script and C++ symbol on one page |

Quantum Script itself (the language, `Script.requireExtension`, the
interpreter options, embedding, writing extensions) is documented in the
`quantum-script` repository, `docs/`.

## Source map

```
source/XYO/QuantumScript.Extension/Application.hpp            umbrella header, include this from C++
source/XYO/QuantumScript.Extension/Application.Amalgam.cpp    the whole extension in one translation unit
source/XYO/QuantumScript.Extension/Application/
    Dependency.hpp                                            <XYO/QuantumScript.hpp>, export macro
    Library[.hpp/.cpp]                                        initExecutive, registerInternalExtension,
                                                              the native getCmdN, getCmdS, getExecutable,
                                                              getPathExecutable
    Library.js                                                script part: Application.arguments,
                                                              getFlagValue, hasFlag, getArgument
    Library.Source.cpp                                        generated from Library.js (make.prepare)
    Copyright / License / Version                             library metadata
fabricare/make.prepare.js                                     runs file-to-cs: Library.js -> Library.Source.cpp
test/test.01.cpp                                              C++ host registering Console and Application as internal
test/test.01.js                                               prints the executable, its folder and every argument
```

## AI assistant skill

A Claude Code skill describing how to use this extension lives in
[`.claude/skills/quantum-script--application/`](../.claude/skills/quantum-script--application/SKILL.md).
It is picked up automatically inside this repository; copy the folder to
`~/.claude/skills/` to have it available in the projects that use
`Application` (Quantum Script tools, fabricare scripts, other extensions).
