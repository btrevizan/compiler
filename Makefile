all: parser.y scanner.l main.c tree.c table.c stack.c checks.c code.c iloc.c backpatching.c address.c
	bison -d parser.y --report-file=report.out -r all
	flex scanner.l
	gcc -c lex.yy.c parser.tab.c lexical.c tree.c table.c stack.c checks.c code.c iloc.c backpatching.c address.c -g
	gcc -o etapa6 lex.yy.o parser.tab.o lexical.o tree.o table.o stack.o checks.o code.o iloc.o backpatching.o address.o main.c -ll -lm

clean:
	rm -f *.o
	rm -f parser.tab.*
	rm -f report.out
	rm -f lex.yy.*
	rm -f etapa6
	rm -f e3.csv
	rm -f leaks
