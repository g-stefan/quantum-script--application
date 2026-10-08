---
name: quantum-script--application
description: >-
  How to use the Quantum Script Application extension
  (quantum-script--application), the command line extension loaded with
  Script.requireExtension("Application"): Application.getCmdN /
  getCmdS(index) (raw argc / argv of the host, index 0 = executable,
  undefined past the end), Application.arguments (argv after the executable),
  hasFlag(name) (--name or --name=...), getFlagValue(name, default)
  (--name=value, everything after the first "="), getArgument(position,
  default) (arguments not starting with --; under quantum-script and .cmd
  launchers position 0 is the script itself), getPathExecutable /
  getExecutable (folder / path of the running executable); what argv looks
  like under quantum-script, --cmd launchers, --run, fabricare and custom
  C++ hosts; registerInternalExtension in a C++ host. Use when writing or
  reviewing Quantum Script or fabricare .js code that reads command line
  arguments or options, writes a command line tool or .cmd launcher, needs
  the executable folder, C++ code that includes
  <XYO/QuantumScript.Extension/Application.hpp>, a fabricare.json depending
  on "quantum-script--application", or when working inside the
  quantum-script--application repository.
---

# quantum-script--application

Command line extension of Quantum Script (see the `quantum-script` skill for
the language and its differences from JavaScript; its rules apply).
Purpose: **give scripts the command line of the host process and the
location of its executable**, plus minimal `--option` parsing, so command
line tools, `.cmd` launchers and fabricare build scripts can be written in
Quantum Script.

Full documentation: `docs/` in the quantum-script--application repository
(`X:\Storage\XYO\Gitea\CPP\quantum-script--application\docs` on this
machine): README, getting-started, **command-line** (what argv contains for
each host, flags vs positional, complete tool, recipes, pitfalls),
**script-api** (exact behavior of every function), reference. Read the
matching page when you need more than this summary. When in doubt read
`source/XYO/QuantumScript.Extension/Application/Library.cpp` (native part,
~110 lines) and `Library.js` (script part, ~60 lines).

## Script API

```javascript
Script.requireExtension("Application");     // built into fabricare; DLL next to quantum-script

Application.getCmdN();                      // argc, argv[0] included
Application.getCmdS(k);                     // argv[k] as typed; undefined when k >= getCmdN()
Application.arguments;                      // ["tool.js", "in.txt", "--out=x"]  = argv[1..], snapshot at load
Application.hasFlag("verbose");             // --verbose or --verbose=... present
Application.getFlagValue("out", "a.txt");   // value of first --out=...; default if absent
Application.getArgument(1, "default");      // 2nd argument not starting with "--"
Application.getPathExecutable();            // "C:\\...\\bin" folder of the running exe, absolute
Application.getExecutable();                // folder + file name of argv[0] as typed
```

## Who is at position 0

| Started as | `getArgument(0)` | user args start at |
|------------|------------------|--------------------|
| `quantum-script tool.js a b` | `"tool.js"` | **1** |
| `tool.cmd a b` (`@quantum-script --cmd "%~f0" %*`) | full path of `tool.cmd` | **1** |
| `quantum-script --run="..." a b` | `"a"` | 0 |
| `fabricare make --x=1` | `"make"` (the action) | 0 |
| own C++ host | whatever `argv` the host passed to `ExecutiveX::initExecutive` | |

The interpreter passes its **own** argv through: its options (`--cmd`,
`--run=...`, `--execution-time`, ...) are visible to `hasFlag`, and
`@file` arguments are **not** expanded (fabricare does expand them).

## Hard rules

1. **`getCmdS(k)` past the end is `undefined`**: `k >= getCmdN()` (or no
   command line at all) returns `undefined`, not `""`. Test with
   `Script.isUndefined`, loop `k < Application.getCmdN()`, or use
   `getArgument(k, default)` when you want a default. Negative / `NaN`
   indexes become `0` (the executable).
2. **Flag = starts with `--`, everything else is positional.** `-v`, `/v`,
   `@file` are positional and count in `getArgument` numbering.
3. **Values need `=`**: `--out file` is flag `--out` (no value) plus
   positional `file`; `getFlagValue("out")` returns the default.
4. **Value is everything after the first `=`**: `--define=A=B` → `"A=B"`.
   `--name=` → `""` (not the default).
5. **First match only** for `getFlagValue`; repeated flags must be collected
   by looping over `Application.arguments`.
6. Missing default → `undefined`; test with `Script.isUndefined(x)`.
7. **Everything is a String**: `Convert.toNumber(Application.getFlagValue("n", "1"))`.
8. Names are case sensitive; no `--` end-of-options marker; `hasFlag("x")`
   is also true for an argument that merely *contains* `--x=`.
9. `Application.arguments` is a plain array built at load time; the helpers
   read it on every call, so `Application.arguments.shift()` (e.g. to drop
   the script name) changes what they see. `getCmdN` / `getCmdS` do not
   change.
10. `getExecutable()` uses `argv[0]` as typed: from `cmd.exe` it may lack
    `.exe`. For locating files use `getPathExecutable()` + `"/name"`
    (forward slash works on Windows). There is no `Shell.pathSeparator`.
11. Avoid option names the interpreter uses (`cmd`, `run`, `compile`,
    `strip`, `add`, `verify`, `execution-time`, `license`, `usage`,
    `version`): the interpreter acts on them too.
12. **One engine per thread**: each thread requires `Application` itself;
    the `Thread` extension starts it with the parent's argv, so values are
    the same, but `Application.arguments` changes are per thread.

## Typical tool

```javascript
Script.requireExtension("Console");
Script.requireExtension("Application");

if (Application.hasFlag("help")) {
	Console.writeLn("usage: quantum-script tool.js input [output] [--force] [--level=n]");
	Script.exit(0);
};
var input = Application.getArgument(1);                        // 0 is tool.js
if (Script.isUndefined(input)) {
	Console.writeLn("Error: input expected");
	Script.exit(1);
};
var output = Application.getArgument(2, input + ".out");
var force = Application.hasFlag("force");
var level = Convert.toNumber(Application.getFlagValue("level", "1"));
```

Files next to the script: `Shell.getFilePath(Application.getArgument(0))`
(under `quantum-script` / `.cmd`). Files next to the exe:
`Application.getPathExecutable() + "/data.json"`.

## C++

```cpp
#include <XYO/QuantumScript.Extension/Application.hpp>
using namespace XYO::QuantumScript;

void initExecutive(Executive *executive) {                  // host init callback
	Extension::Application::registerInternalExtension(executive);
};
// pass the host's real argc / argv: they are what scripts see
ExecutiveX::initExecutive(cmdN, cmdS, initExecutive);
```

- To hide host options or expand `@file`, build a new argv (keep `argv[0]`,
  `getExecutable` uses its file name) and pass that — fabricare does this.
- Values come from `Executive::mainCmdN`, `mainCmdS`, `pathExecutable`
  (set by `ExecutiveX::initExecutive`).
- Static build: depend on `quantum-script--application.static` (exports
  `XYO_QUANTUMSCRIPT_EXTENSION_APPLICATION_LIBRARY`, no
  `quantumScriptExtension` entry point) and register it as internal.
- Scripts still call `Script.requireExtension("Application")`; an external
  DLL on the include path wins, `Script.requireInternalExtension` forces the
  internal one.

## Working in this repository

- Build: `fabricare make`, `fabricare test` (runs `test/test.01`, which
  registers Console and Application as internal extensions and runs
  `test/test.01.js`; run `make` first), `fabricare install` (see the
  `fabricare` skill). `quantum-script` and `quantum-script--console` must be
  installed first; `file-to-cs` must be on the `PATH`.
- Two halves: native functions in `Application/Library.cpp`
  (`static TPointer<Variable> name(VariableFunction *function, Variable *this_, VariableArray *arguments)`,
  the `Executive *` is `function->valueSuper`, registered with
  `executive->setFunction4("Application.name(args)", name, executive)`), and
  script functions in `Application/Library.js`. `fabricare/make.prepare.js`
  converts `Library.js` to `Library.Source.cpp` (`librarySource`), compiled
  by `initExecutive` after the native functions; never edit the generated
  file.
- New functions: update `README.md`, `docs/script-api.md`,
  `docs/reference.md` and this skill.
- Code style: tabs (width 8), `.clang-format`, CRLF, statements and blocks
  end with `};`, camelCase. SPDX header: MIT for `source/` and `docs/`,
  Unlicense for `test/` and `.claude/` (see `.reuse/dep5`).
