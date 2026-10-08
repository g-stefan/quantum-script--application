# API reference

## Script

Available after `Script.requireExtension("Application")`. Details in
[Script API](script-api.md).

### Native functions

| Symbol | Returns | Notes |
|--------|---------|-------|
| `Application.getCmdN()` | Number | `argc` of the host, `argv[0]` included |
| `Application.getCmdS(index)` | String / `undefined` | `argv[index]`; `undefined` when `index >= getCmdN()` or there is no command line; invalid index → `0` |
| `Application.getPathExecutable()` | String | absolute, resolved folder of the running executable, no trailing separator |
| `Application.getExecutable()` | String | `getPathExecutable()` + separator + file name of `argv[0]` as typed; only `getPathExecutable()` when there is no `argv[0]` |

### Script functions and properties

| Symbol | Returns | Notes |
|--------|---------|-------|
| `Application.arguments` | Array of String | `argv[1 ..]`, built at load time; may be changed, helpers read it at each call |
| `Application.hasFlag(name)` | Boolean | an argument equals `--name` or contains `--name=` |
| `Application.getFlagValue(name, defaultValue)` | String / `defaultValue` | first `--name=value`; value is everything after the first `=`; `--name=` → `""` |
| `Application.getArgument(position, defaultValue)` | String / `defaultValue` | `position`-th argument not starting with `--` (0 based) |

### Argument classification

| Argument | Kind |
|----------|------|
| starts with `--`, contains `=` | flag with value (`hasFlag`, `getFlagValue`) |
| starts with `--`, no `=` | flag without value (`hasFlag`) |
| anything else (`file`, `-v`, `/v`, `@file`) | positional (`getArgument`) |

### Position 0 by host

| Started as | `getArgument(0)` | First user argument |
|------------|------------------|---------------------|
| `quantum-script script.js args...` | `script.js` | `getArgument(1)` |
| `script.cmd args...` (`quantum-script --cmd "%~f0" %*`) | full path of `script.cmd` | `getArgument(1)` |
| `quantum-script --run="code" args...` | first of `args` | `getArgument(0)` |
| `fabricare action ...` | the action | `getArgument(0)` |
| your own host | depends on the `argv` the host passes | |

## C++

```cpp
#include <XYO/QuantumScript.Extension/Application.hpp>
```

Namespace `XYO::QuantumScript::Extension::Application`.

| Symbol | Purpose |
|--------|---------|
| `void registerInternalExtension(Executive *executive)` | make `Application` available as an internal extension; call from the host's init callback |
| `void initExecutive(Executive *executive, void *extensionId)` | the extension initializer: creates `Application`, the native functions, and runs `Library.js` |
| `extern "C" void quantumScriptExtension(Executive *, void *)` | DLL entry point used by `Script.requireExtension` (DLL build only) |
| `Version::version()`, `Version::build()`, `Version::versionWithBuild()`, `Version::datetime()` | library version |
| `Copyright::copyright()`, `License::license()`, `License::shortLicense()` | library metadata |

The engine fields read by the native functions (`Executive` of
`quantum-script`):

| Field | Set by | Used by |
|-------|--------|---------|
| `int mainCmdN` | `ExecutiveX::initExecutive(cmdN, cmdS, ...)` | `getCmdN`, `Application.arguments` |
| `char **mainCmdS` | `ExecutiveX::initExecutive(cmdN, cmdS, ...)` | `getCmdS`, `getExecutable` |
| `String pathExecutable` | `ExecutiveX::initExecutive`, from `Shell::getExecutablePath()` + `Shell::realPath` | `getPathExecutable`, `getExecutable` |

### Build projects (`fabricare.json`)

| Project | Kind | Dependencies | Exports to consumers |
|---------|------|--------------|----------------------|
| `quantum-script--application` | `dll-or-lib` | `quantum-script`, `quantum-script--console` | |
| `quantum-script--application.static` | `lib`, static CRT | `quantum-script.static`, `quantum-script--console.static` | define `XYO_QUANTUMSCRIPT_EXTENSION_APPLICATION_LIBRARY` |
| `test.01` | `exe`, test | `quantum-script--application` | |

### Macros

| Macro | Meaning |
|-------|---------|
| `XYO_QUANTUMSCRIPT_EXTENSION_APPLICATION_INTERNAL` | defined while building the DLL itself: export symbols |
| `XYO_QUANTUMSCRIPT_EXTENSION_APPLICATION_LIBRARY` | static library: empty export macro, no `quantumScriptExtension` |
| `XYO_QUANTUMSCRIPT_EXTENSION_APPLICATION_EXPORT` | export / import / empty, from the two above |
| `XYO_QUANTUMSCRIPT_DEBUG_RUNTIME` | print a trace line on every native call |
