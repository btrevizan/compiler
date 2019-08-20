# Compiler
This is a compiler project for Compilers class at UFRGS.

We use `flex` and `bison`, so you must install them before running anything.

After installation, there is a `makefile` that you can use to build the project, just:

```
$ make all
```

To test the application, you should execute:
```
$ make test
```

## Part \#1
This part should implement a lexical analysis using regular expressions defined on `scanner.l` and compiled by `flex`.
