# Quantum Script Extension Application

Quantum Script extension
- Access to the command line of the program running the script: argument
count and values (`getCmdN`, `getCmdS`), `Application.arguments`.
- Simple option parsing: `--flag` (`hasFlag`), `--name=value`
(`getFlagValue`), positional arguments (`getArgument`).
- Location of the running executable (`getPathExecutable`, `getExecutable`),
to find files installed next to it.
- Used by command line tools and `.cmd` launchers written in Quantum Script,
and by `fabricare` build scripts.

```javascript
Script.requireExtension("Application");

Application;
Application.getCmdN();
Application.getCmdS(index);
Application.getExecutable();
Application.getPathExecutable();
Application.arguments;
Application.getFlagValue(argument,defaultValue);
Application.hasFlag(argument);
Application.getArgument(position,defaultValue);
```

Built on `quantum-script`, part of the XYO C++ SDK.

## Documentation

- [Overview](docs/README.md) - purpose and design
- [Getting started](docs/getting-started.md) - build, first script, `.cmd` launchers, fabricare, register in a C++ host, static builds
- [Command line model](docs/command-line.md) - what scripts see under each host, flags vs positional arguments, recipes, pitfalls
- [Script API](docs/script-api.md) - every function: exact behavior, edge cases
- [API reference](docs/reference.md)

A Claude Code skill for this extension is in
[.claude/skills/quantum-script--application](.claude/skills/quantum-script--application/SKILL.md).

## License

Copyright (c) 2016-2026 Grigore Stefan
Licensed under the [MIT](LICENSE) license.
