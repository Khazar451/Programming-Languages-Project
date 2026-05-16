# Programming Languages Project

A C-based lexical analyzer and recursive-descent parser for a small educational programming language with Turkish-inspired keywords.

## Overview

This project demonstrates core compiler front-end concepts:
- **Lexical analysis** (tokenizing source input)
- **Syntax analysis** (parsing with recursive-descent functions)
- **Basic symbol table tracking** for identifiers

Two executables are included:
- `lexer.c`: standalone lexical analyzer
- `parser.c`: integrated lexer + parser with grammar validation

## Repository Structure

- `lexer.c` - standalone lexer implementation
- `parser.c` - lexer + recursive-descent parser
- `Arithmetic_Expressions.txt` - valid input sample
- `Conditional_Statements.txt` - valid input sample
- `Loop_Structures.txt` - valid input sample
- `Variable_Declaration_and_Assigments.txt` - valid input sample
- `Invalid_File.txt` - invalid input sample for error handling

### Supported Keywords

- `tam_sayı` (integer type declaration)
- `eğer` / `değilse` / `ise` (if / else / else-if style flow)
- `iken` (while)
- `için` (for)
- `yazdır` (print token recognized by lexer)

### Supported Constructs

- Variable declaration: `tam_sayı x;`
- Assignment: `x = 10;`
- Arithmetic expressions: `+`, `-`, `*`, `/`
- Conditions with relational operators: `<`, `>`, `==`, `!=`, `<=`, `>=`
- Conditional blocks via `eğer`, `ise`, `değilse`
- Loops via `iken` and `için`
- Increment/decrement forms in loop operations: `i++`, `i--`, `++i`, `--i`

## Requirements

- A C compiler (`gcc` or `clang`)
- Bash-compatible terminal

## Build

From the repository root:

```bash
# Build parser
gcc -Wall -Wextra -std=c11 parser.c -o parser

# Build lexer
gcc -Wall -Wextra -std=c11 lexer.c -o lexer
```

(You can replace `gcc` with `clang`.)

## Usage

### Run the Parser

```bash
./parser <input_file>
```

Examples:

```bash
./parser Arithmetic_Expressions.txt
./parser Conditional_Statements.txt
./parser Loop_Structures.txt
./parser Variable_Declaration_and_Assigments.txt
./parser Invalid_File.txt
```

### Run the Lexer

```bash
./lexer <input_file>
```

Example:

```bash
./lexer Arithmetic_Expressions.txt
```

## Suggested Test Inputs

### Valid Files
- `Arithmetic_Expressions.txt`
- `Conditional_Statements.txt`
- `Loop_Structures.txt`
- `Variable_Declaration_and_Assigments.txt`

### Invalid File
- `Invalid_File.txt`

## Notes

- The parser reports syntax errors when input does not match the expected grammar.
- The parser prints a final symbol table of encountered identifiers.
- The project is intended for educational/compiler-course use.
