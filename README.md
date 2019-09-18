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

### Part \#1
This part should implement a lexical analysis using regular expressions defined on `scanner.l` and compiled by `flex`.

### Part \#2
This part should implement a syntax analysis using grammar definitions written on `parser.y` and compiled by `bison`.

### Part \#3
This part implements an Abstract Syntax Tree (AST) using `bison`. 

# Warning!!!
If you're using **Mac OS**, the `flex` library flag is **-ll**.

If you're using **Linux**, the `flex` library flag is **-lfl**.