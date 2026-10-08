# Command line model

`Application` does not interpret the command line on its own: it returns what
the **host program** (the executable that embeds the engine) received, and
applies a few simple rules on top. To use it correctly you need to know what
the host puts there.

## Where the values come from

| Value | Source |
|-------|--------|
| `getCmdN()`, `getCmdS(k)` | `argc` / `argv` the host passed to `ExecutiveX::initExecutive`, stored in `Executive::mainCmdN` / `mainCmdS` |
| `Application.arguments` | `getCmdS(1) .. getCmdS(getCmdN() - 1)`, copied once when the extension is loaded |
| `getPathExecutable()` | `Executive::pathExecutable`: the real (resolved) folder of the running executable, found by the OS, not from `argv` |
| `getExecutable()` | `getPathExecutable()` + path separator + file name part of `argv[0]` |

## What the command line looks like

### `quantum-script script.js ...`

The interpreter passes its **own** `argv` unchanged. Its options and the
script name stay in it:

```bash
quantum-script tool.js input.txt --out=result.txt
```

| Call | Result |
|------|--------|
| `getCmdS(0)` | `quantum-script` / `C:\...\quantum-script.exe` (as typed / as given by the shell) |
| `Application.arguments` | `["tool.js", "input.txt", "--out=result.txt"]` |
| `getArgument(0)` | `"tool.js"` — the script itself |
| `getArgument(1)` | `"input.txt"` — the first user argument |
| `getFlagValue("out")` | `"result.txt"` |

Interpreter options are visible too: with
`quantum-script --execution-time tool.js`, `hasFlag("execution-time")` is
`true`. Avoid naming your own options like the interpreter's (`--cmd`,
`--run`, `--compile`, `--strip`, `--add`, `--verify`, `--execution-time`,
`--execution-time-cmd`, `--license`, `--usage`, `--version`): the
interpreter acts on them too, and `--license` / `--usage` / `--version` stop
it before the script runs.

`@file` arguments are **not** expanded for the script: the interpreter reads
`@file` to find its own options, but `Application` sees the literal
`"@file"` string.

### `.cmd` launcher (`quantum-script --cmd "%~f0" %*`)

```bat
greet.cmd World --times=2
```

runs `quantum-script --cmd C:\full\path\greet.cmd World --times=2`:

| Call | Result |
|------|--------|
| `Application.arguments` | `["--cmd", "C:\\full\\path\\greet.cmd", "World", "--times=2"]` |
| `getArgument(0)` | `"C:\\full\\path\\greet.cmd"` (full path, because of `%~f0`) |
| `getArgument(1)` | `"World"` |
| `hasFlag("cmd")` | `true` |

So the positional numbering is the same as for `quantum-script script.js`:
the script is position 0, user arguments start at 1.

### `quantum-script --run="code" ...`

There is no script file. The `--run=...` argument is skipped by
`getArgument` (it starts with `--`), so the first user argument is position
**0**:

```bash
quantum-script --run="Script.requireExtension('Console');Script.requireExtension('Application');Console.writeLn(Application.getArgument(0));" hello
```

prints `hello`.

### `fabricare`

`fabricare` expands `@file` arguments and then starts the engine with
`argv[0]` followed by the expanded arguments. Positional 0 is the action:

```bash
fabricare make --target=library
```

| Call | Result |
|------|--------|
| `getArgument(0)` | `"make"` |
| `getFlagValue("target")` | `"library"` |
| `getPathExecutable()` | folder of `fabricare.exe` |

### Your own C++ host

Whatever `argv` you pass to `ExecutiveX::initExecutive` is what scripts see.
If the host itself takes the script name as its first argument, scripts will
see it as `getArgument(0)` just like under `quantum-script`; if the host runs
a fixed, embedded script, the first user argument is `getArgument(0)`.
Document which one applies for your host.

## Flags and positional arguments

`Application` splits arguments in two kinds by one rule: **an argument that
starts with `--` is a flag, anything else is positional.**

| Argument | Kind | Seen by |
|----------|------|---------|
| `--verbose` | flag without value | `hasFlag("verbose")` |
| `--out=file.txt` | flag with value | `hasFlag("out")`, `getFlagValue("out")` |
| `--out=` | flag with empty value | `getFlagValue("out")` returns `""` |
| `file.txt` | positional | `getArgument(k)` |
| `-v`, `/v` | **positional** (single dash and slash options are not flags) | `getArgument(k)` |
| `--` | flag with an empty name | nothing useful |

Rules that follow from the implementation (see [Script API](script-api.md)
for every detail):

- **Values need `=`.** `--out file.txt` is the flag `--out` without a value
  followed by the positional argument `file.txt`. `getFlagValue("out")`
  returns the default.
- **The value is everything after the first `=`.** `--define=A=B` gives
  `getFlagValue("define") == "A=B"`.
- **The first match wins** for `getFlagValue`; repeated flags are not
  collected. Loop over `Application.arguments` to collect them.
- **Names are case sensitive**: `--Verbose` is not `--verbose`.
- **There is no `--` end-of-options marker**: every later argument that
  starts with `--` is still a flag.
- `hasFlag("x")` is also true for any argument that merely *contains*
  `--x=` (for example the positional `a--x=1`).

## A complete tool

`copy-upper.js`: convert a text file to upper case.

```javascript
Script.requireExtension("Console");
Script.requireExtension("Application");
Script.requireExtension("Shell");

function usage() {
	Console.writeLn("usage: quantum-script copy-upper.js input [output] [--force]");
	Console.writeLn("       --force    overwrite output if it exists");
};

if (Application.hasFlag("help")) {
	usage();
	Script.exit(0);
};

var input = Application.getArgument(1);                 // 0 is copy-upper.js
if (Script.isUndefined(input)) {
	usage();
	Script.exit(1);
};
var output = Application.getArgument(2, input + ".upper");
var force = Application.hasFlag("force");

if (Shell.fileExists(output) && !force) {
	Console.writeLn("Error: " + output + " exists, use --force");
	Script.exit(1);
};

var text = Shell.fileGetContents(input);
if (Script.isUndefined(text)) {
	Console.writeLn("Error: cannot read " + input);
	Script.exit(1);
};
Shell.filePutContents(output, text.toUpperCaseASCII());
```

## Recipes

Every value of a repeated flag (`--include=a --include=b`):

```javascript
function getFlagValues(name) {
	var prefix = "--" + name + "=";
	var retV = [];
	var scan;
	for (scan of Application.arguments) {
		if (scan.indexOf(prefix) == 0) {
			retV.push(scan.substring(prefix.length));
		};
	};
	return retV;
};
```

All positional arguments after the script name:

```javascript
var files = [];
var k = 1;                                   // 0 is the script under quantum-script
var arg;
while (!Script.isUndefined(arg = Application.getArgument(k))) {
	files.push(arg);
	++k;
};
```

Files installed next to the executable:

```javascript
var config = Application.getPathExecutable() + "/tool.json";   // "/" works on Windows too
```

Files next to the script (under `quantum-script` or a `.cmd` launcher):

```javascript
Script.requireExtension("Shell");
var scriptPath = Shell.getFilePath(Application.getArgument(0));
```

## Pitfalls

- **`getCmdS(k)` past the end is `undefined`.** An index `>= getCmdN()`
  returns `undefined`, not an empty string; test with `Script.isUndefined`,
  loop with `k < Application.getCmdN()`, or use `getArgument(k, default)`
  when you want a default. The index is converted to a number (`"2"` is
  `2`); negative, `NaN`, infinite and missing indexes become `0` (so they
  return the executable), fractions are truncated.
- **`getArgument(0)` is the script** under `quantum-script` and `.cmd`
  launchers, not the first user argument.
- **`getExecutable()` uses `argv[0]` as typed.** Started from `cmd.exe` as
  `quantum-script`, it is `...\bin\quantum-script` without `.exe`. Use
  `getPathExecutable()` when you only need the folder.
- **Everything is a string.** Convert numbers yourself:
  `Convert.toNumber(Application.getFlagValue("times", "1"))`.
- **`Application.arguments` is a snapshot.** It is built when the extension
  loads; the helpers read it at each call, so changing it (for example
  `Application.arguments.shift()` to drop the script name) changes what
  `hasFlag`, `getFlagValue` and `getArgument` see. `getCmdN` / `getCmdS` are
  not affected.
