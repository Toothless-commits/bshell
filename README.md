# Custom Unix Shell

A Unix-like command-line shell implemented in C++, designed to replicate core shell functionality while providing a modular foundation for advanced operating system concepts. The project focuses on command parsing, process management, inter-process communication, and shell environment handling.

This shell serves as both a learning platform for systems programming and a framework for implementing features commonly found in modern Unix shells.

---

## Overview

The shell provides an interactive command-line interface that accepts user commands, parses input, and executes programs using operating system primitives. The implementation emphasizes clean architecture, extensibility, and adherence to Unix process execution models.

---

## Current Capabilities

- Interactive shell prompt
- Command parsing and tokenization
- Support for whitespace-separated arguments
- Single-quoted string handling (`'...'`)
- Double-quoted string handling (`"..."`)
- Escaped character processing (`\`)
- External command execution
- Basic shell loop and process management

---

## Planned Features

The project is actively being expanded to support additional shell functionality, including:

### Command Execution
- Built-in commands (`cd`, `pwd`, `echo`, `exit`, etc.)
- Environment variable support
- Command substitution

### Process Management
- Foreground and background execution
- Job control
- Signal handling
- Process status tracking

### Redirection
- Input redirection (`<`)
- Output redirection (`>`)
- Append redirection (`>>`)
- Error redirection (`2>`)

### Inter-Process Communication
- Pipes (`|`)
- Multiple command pipelines
- Combined redirection and piping

### User Experience
- Command history
- History expansion
- Auto-completion
- Prompt customization

### Shell Features
- Aliases
- Environment variable expansion
- Configuration file support
- Scripting support

---

## Architecture

The shell is organized into independent components to simplify development and future extensions.

