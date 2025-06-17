# C development repository

This repository contains my code explorations in C.

## Building, running and testing

To build the code, use: `build.sh`.

To build and run the code, use `run.sh`.

To run the unit tests, use `test.sh`.

To delete all the generated files, use `clean.sh`.

You can also use `just` to build, run, test and clean also.

## AI Rules

Some rules for AI generation:

- Use British spelling in naming and comments.
- Use `build.sh` to build the code.
- Use `test.sh` to run the unit tests.  All unit test source code start with `test_`.
- Use `run.sh` to run the application.
- The standard for C in use is C23.
- All generated files must be under a folder whose name starts with an underscore.
- Use clang for compilation.
