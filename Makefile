all: parser.y scanner.l main.c
	bison -d parser.y --report-file=report.out -r all
	flex scanner.l
	gcc -c lex.yy.c parser.tab.c tree.c
	gcc -o etapa3 lex.yy.o parser.tab.o tree.o main.c -ll

test: test.txt etapa3
	cat test.txt | ./etapa3

clean:
	rm -f *.o
	rm -f parser.tab.*
	rm -f report.out
	rm -f lex.yy.*
	rm -f etapa3
