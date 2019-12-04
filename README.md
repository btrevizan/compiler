# Compiler
This is a compiler project for Compilers class at UFRGS.

We use `flex` and `bison`, so you must install them before running anything.

After installation, there is a `makefile` that you can use to build the project, just:

```
$ make
```

To run the coompiler over a source code, just type:
```
$ ./etapa6 < source_code > out.iloc
```

And to run the `out.iloc` file:
```
$ python3 ilocsim.py -x -t -s --data 500 --stack 1024 out.iloc
```

### Part \#1
This part should implement a lexical analysis using regular expressions defined on `scanner.l` and compiled by `flex`.

### Part \#2
This part should implement a syntax analysis using grammar definitions written on `parser.y` and compiled by `bison`.

### Part \#3
This part implements an Abstract Syntax Tree (AST) using `bison`. 

### Part \#4
This part uses the AST created to check for semantic errors.

### Part \#5
Code generation in ILOC.

### Part \#6
Code generation with function support.

# Warning!!!
If you're using **Mac OS**, the `flex` library flag is **-ll**.

If you're using **Linux**, the `flex` library flag is **-lfl**. 
In some distributions, however, **-ll** works fine.