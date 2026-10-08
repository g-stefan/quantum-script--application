# Getting started

## 1. Build and install

The extension is built with [fabricare](https://github.com/g-stefan/fabricare),
the build tool used by all XYO C++ projects. `quantum-script` (and everything
below it: `xyo-system`, `xyo-encoding`, ...) and `quantum-script--console`
must be installed to the SDK first, and the `file-to-cs` tool must be on the
`PATH`. From the repository root:

```bash
fabricare make       # build into output/
fabricare test       # build and run test/test.01 (run make first)
fabricare install    # copy output/{bin,include,lib} to ~/.fabricare/<platform>
fabricare clean      # remove output/ and temp/
```

Before compiling, `fabricare/make.prepare.js` runs `file-to-cs` to turn the
script half of the extension, `Library.js`, into the C++ string
`librarySource` in `Library.Source.cpp`. Edit `Library.js`, never the
generated file.

Two libraries are produced:

| Project                              | Kind                                | Use it when                                         |
|--------------------------------------|-------------------------------------|-----------------------------------------------------|
| `quantum-script--application`        | DLL / shared library (`dll-or-lib`) | scripts run by `quantum-script`, or a host using the engine DLL |
| `quantum-script--application.static` | static library, static CRT          | self-contained hosts built with `quantum-script.static` |

After `fabricare install`, `quantum-script--application.dll` (Windows) /
`libquantum-script--application.so` (Linux) sits in the SDK `bin` folder next
to `quantum-script.exe`, which is where `Script.requireExtension("Application")`
finds it.

## 2. First script

`args.js`:

```javascript
Script.requireExtension("Console");
Script.requireExtension("Application");

Console.writeLn("running: " + Application.getExecutable());
Console.writeLn("from:    " + Application.getPathExecutable());

var k;
for (k = 0; k < Application.getCmdN(); ++k) {
	Console.writeLn(k + ": " + Application.getCmdS(k));
};

Console.writeLn("input:   " + Application.getArgument(1, "(none)"));
Console.writeLn("output:  " + Application.getFlagValue("out", "out.txt"));
Console.writeLn("verbose: " + Application.hasFlag("verbose"));
```

```bash
quantum-script args.js data.txt --out=result.txt --verbose
```

```
running: C:\Users\me\.fabricare\win64-msvc-2026\bin\quantum-script.exe
from:    C:\Users\me\.fabricare\win64-msvc-2026\bin
0: C:\Users\me\.fabricare\win64-msvc-2026\bin\quantum-script.exe
1: args.js
2: data.txt
3: --out=result.txt
4: --verbose
input:   data.txt
output:  result.txt
verbose: true
```

Note that `getArgument(0)` is `args.js`: the interpreter passes its own
command line through unchanged, and the script file name is the first
argument that does not start with `--`. The first argument meant for the
script is `getArgument(1)`. [Command line model](command-line.md) explains
what the command line looks like for every way of starting a script.

`Script.requireExtension("Application")` looks for an external
`quantum-script--application` library first (in the include path: next to the
script, next to the interpreter), then for an internal extension registered by
the host. Loading twice does nothing. A missing extension throws
`Unable to open "Application"`.

## 3. A Windows command written in Quantum Script

With `--cmd` the interpreter skips the first two lines of the file, so a
`.cmd` file can run itself and forward its arguments with `%*`:

```bat
@quantum-script --cmd "%~f0" %*
@exit /b %ERRORLEVEL%
Script.requireExtension("Console");
Script.requireExtension("Application");

if (Application.hasFlag("help")) {
	Console.writeLn("usage: greet name [--times=n]");
	Script.exit(0);
};

var name = Application.getArgument(1);          // 0 is this .cmd file
if (Script.isUndefined(name)) {
	Console.writeLn("Error: name expected");
	Script.exit(1);
};
var times = Convert.toNumber(Application.getFlagValue("times", "1"));
var k;
for (k = 0; k < times; ++k) {
	Console.writeLn("Hello, " + name + "!");
};
```

```bat
greet World --times=2
```

The command line seen by the script is
`quantum-script --cmd C:\full\path\greet.cmd World --times=2`, so
`getArgument(0)` is the full path of `greet.cmd` (handy to find files next to
it with `Shell.getFilePath`), and `hasFlag("cmd")` is `true`.

## 4. fabricare build scripts

`fabricare` registers `Application` as an internal extension and requires it
before running its own scripts, so build scripts can call it directly:

```javascript
// fabricare/make.js of some project
if (Application.hasFlag("debug")) {
	messageAction("make [debug]");
};
var target = Application.getFlagValue("target", "all");
```

```bash
fabricare make --target=library --debug
```

Under `fabricare`, `getArgument(0)` is the action (`make`, `test`,
`install`, ...), and `getPathExecutable()` is the folder of `fabricare.exe`
(fabricare uses it to find its platform files). Arguments read from
`@file` are already expanded by fabricare.

## 5. Register it in a C++ host

A host that embeds Quantum Script makes `Application` available as an internal
extension by registering it in the init callback. The host's own `argc` /
`argv` are what the extension returns, so pass them to
`ExecutiveX::initExecutive` unchanged (this is what `test/test.01.cpp` does):

```cpp
#include <XYO/QuantumScript.hpp>
#include <XYO/QuantumScript.Extension/Console.hpp>
#include <XYO/QuantumScript.Extension/Application.hpp>

using namespace XYO::QuantumScript;

void initExecutive(Executive *executive) {
	Extension::Console::registerInternalExtension(executive);
	Extension::Application::registerInternalExtension(executive);
};

int main(int cmdN, char *cmdS[]) {
	int exitCode = 1;
	if (ExecutiveX::initExecutive(cmdN, cmdS, initExecutive)) {
		if (ExecutiveX::executeString(
		        "Script.requireExtension(\"Console\");"
		        "Script.requireExtension(\"Application\");"
		        "Console.writeLn(Application.getArgument(0, \"no arguments\"));")) {
			exitCode = ExecutiveX::getExitCode();
		} else {
			printf("%s\n", (ExecutiveX::getError()).value());
			printf("%s", (ExecutiveX::getStackTrace()).value());
		};
		ExecutiveX::endProcessing();
	};
	return exitCode;
};
```

`my-host one two` prints `one`: here no script name is on the command line,
so the first positional argument is the first user argument.

To hide some of the host's own options from scripts, or to expand `@file`
arguments first, build a new `argv` and pass that (`fabricare` does this,
keeping `argv[0]`). Keep `argv[0]` meaningful: `getExecutable()` is built from
its file name.

Registering only makes the extension *available*: scripts still call
`Script.requireExtension("Application")`, or the host runs that line itself
before the user's script. With the DLL build of the engine an external
`quantum-script--application.dll` found on the include path wins over the
internal one for `requireExtension`; use
`Script.requireInternalExtension("Application")` to force the internal one.

In the host's `fabricare.json`:

```json
{
	"name": "my-host",
	"make": "exe",
	"sourcePath": "XYO/MyHost",
	"dependency": [
		"quantum-script--application"
	]
}
```

## 6. Static builds

For a self-contained executable depend on the static variants and the static
CRT:

```json
{
	"name": "my-host.static",
	"make": "exe",
	"sourcePath": "XYO/MyHost",
	"dependency": [
		"quantum-script.static",
		"quantum-script--console.static",
		"quantum-script--application.static"
	],
	"crt": "static"
}
```

`quantum-script--application.static` exports the define
`XYO_QUANTUMSCRIPT_EXTENSION_APPLICATION_LIBRARY` to its consumers, which
turns `XYO_QUANTUMSCRIPT_EXTENSION_APPLICATION_EXPORT` into nothing and leaves
out the `quantumScriptExtension` DLL entry point. A static host must register
the extension with `registerInternalExtension` (section 5): external DLLs
cannot be loaded into a host that does not use the engine DLL.

## 7. Threads

Each thread that runs scripts has its own engine, so every thread loads the
extension itself with `Script.requireExtension("Application")`. The `Thread`
extension starts the new engine with the same command line as the parent, so
`getCmdN`, `getCmdS`, `Application.arguments` and the helpers return the same
values in every thread. `Application.arguments` is rebuilt per thread from the
command line: changes a script makes to it are not seen by other threads.
