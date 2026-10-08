# Script API

```javascript
Script.requireExtension("Application");
```

Loading the extension creates the global object `Application` (a plain
object, `var Application = {};`), adds four native functions written in C++
and runs the script half of the extension (`Library.js`), which builds
`Application.arguments` and defines the three option helpers. All values are
strings or numbers; nothing here changes the process or the command line.

Throughout this page the examples assume:

```bash
quantum-script tool.js input.txt --out=result.txt --verbose -v --define=A=B
```

## Native functions

### `Application.getCmdN()`

Returns the number of command line arguments the host received (`argc`),
**including** `argv[0]`, the executable. Arguments are ignored.

```javascript
Application.getCmdN();          // 7
```

`0` only in a host that started the engine without a command line.

### `Application.getCmdS(index)`

Returns `argv[index]` as a String, exactly as the host received it.

```javascript
Application.getCmdS(0);         // "quantum-script" or "C:\\...\\quantum-script.exe"
Application.getCmdS(1);         // "tool.js"
Application.getCmdS(3);         // "--out=result.txt"
```

- `index` is converted with the engine's index rule: numbers and numeric
  strings are used as is (fractions truncated); negative, `NaN`, infinite or
  missing values become `0`.
- An index past the end (`index >= getCmdN()`) returns `undefined`, the
  same as `getArgument` without a default; so does any index when the host
  started the engine without a command line. Test with
  `Script.isUndefined(value)`, or loop with `index < Application.getCmdN()`.

### `Application.getPathExecutable()`

Returns the folder that contains the running executable: absolute, resolved
(`realPath`), without a trailing separator. It is found by asking the
operating system for the executable's path, so it does not depend on
`argv[0]` or on the current directory.

```javascript
Application.getPathExecutable();   // "C:\\Users\\me\\.fabricare\\win64-msvc-2026\\bin"
```

Under `quantum-script` this is the interpreter's folder (where the SDK
extensions are), under `fabricare` the folder of `fabricare.exe`, in your own
host the folder of your executable.

### `Application.getExecutable()`

Returns `getPathExecutable()` + the platform path separator + the file name
part of `argv[0]`.

```javascript
Application.getExecutable();       // "C:\\Users\\me\\.fabricare\\win64-msvc-2026\\bin\\quantum-script.exe"
```

The file name is taken from `argv[0]` **as typed**: when a Windows user
types `quantum-script` in `cmd.exe`, the result ends in `quantum-script`
without `.exe`. That still works to start the program again with
`Shell.execute` / `Shell.system`; append `.exe` yourself if you need the
real file. When the host passed no `argv[0]`, the result is just
`getPathExecutable()`.

## Script functions

### `Application.arguments`

Array of Strings: `getCmdS(1) .. getCmdS(getCmdN() - 1)`, i.e. every argument
after the executable, in order.

```javascript
Application.arguments;
// ["tool.js", "input.txt", "--out=result.txt", "--verbose", "-v", "--define=A=B"]
```

- Built once, when the extension is loaded. It is an ordinary array: the
  script may change it, and `hasFlag`, `getFlagValue` and `getArgument` read
  the current content at each call. Elements must stay Strings.
- Not expanded in any way: no `@file` reading, no quote processing beyond
  what the operating system / host did, no wildcard expansion.

### `Application.hasFlag(name)`

Returns `true` if some element of `Application.arguments` is exactly
`"--" + name`, or contains `"--" + name + "="`; otherwise `false`.

```javascript
Application.hasFlag("verbose");    // true  (--verbose)
Application.hasFlag("out");        // true  (--out=result.txt)
Application.hasFlag("v");          // false (-v is not a flag)
Application.hasFlag("Verbose");    // false (case sensitive)
```

The second test is "contains", not "starts with": the positional argument
`a--out=1` also makes `hasFlag("out")` true.

### `Application.getFlagValue(name, defaultValue)`

Returns the value of the first argument of the form `--name=value`, or
`defaultValue` (`undefined` if omitted) when there is none.

```javascript
Application.getFlagValue("out");             // "result.txt"
Application.getFlagValue("out", "a.txt");    // "result.txt"
Application.getFlagValue("mode", "fast");    // "fast"        (no --mode=...)
Application.getFlagValue("verbose", "no");   // "no"          (--verbose has no "=")
Application.getFlagValue("define");          // "A=B"         (everything after the first "=")
```

Exact algorithm, for each element `scan` of `Application.arguments`:

1. if it starts with `"--" + name + "="`, return the rest of it.

Consequences:

- `--name=` returns `""` (an empty String, not the default).
- `--name=a=b` returns `"a=b"`.
- `--name value` (space) is not a value: `getFlagValue` returns the default
  and `value` is a positional argument.
- The value is always a String: convert with `Convert.toNumber(...)` etc.
- Only the first `--name=...` is returned.

### `Application.getArgument(position, defaultValue)`

Returns the positional argument number `position` (0 based), counting only
the elements of `Application.arguments` that **do not** start with `--`, or
`defaultValue` (`undefined` if omitted) when there are not enough.

```javascript
Application.getArgument(0);            // "tool.js"
Application.getArgument(1);            // "input.txt"
Application.getArgument(2);            // "-v"
Application.getArgument(3, "none");    // "none"
```

- Under `quantum-script` and `.cmd` launchers position 0 is the script file;
  user arguments start at 1. Under `quantum-script --run=...`, `fabricare`
  and hosts that run a fixed script, user arguments start at 0. See
  [Command line model](command-line.md).
- Flags can appear anywhere; they do not shift the numbering.
- `position` is compared with `==`, so `"1"` works as `1`.

## Extending `Application`

`Application` is an ordinary object, so a script or a library can add its own
helpers:

```javascript
Application.getFlagNumber = function(name, defaultValue) {
	var value = Application.getFlagValue(name);
	if (Script.isUndefined(value)) {
		return defaultValue;
	};
	return Convert.toNumber(value);
};
```

## Extension information

| Item | Value |
|------|-------|
| Name (for `requireExtension`) | `Application` |
| External library | `quantum-script--application.dll` / `libquantum-script--application.so` |
| Version | `Script.getExtensionList()` shows it; see `version.json` |
| Public | yes |
| Dependencies | none at script level (does not load other extensions) |
