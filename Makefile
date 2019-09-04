all: parser.y scanner.l main.c
	bison -d parser.y --report-file=report.out -r all
	flex scanner.l
	gcc -c lex.yy.c parser.tab.c
	gcc -o etapa2 lex.yy.o parser.tab.o main.c -lfl

test: test.txt etapa2
	cat test.txt | ./etapa2

clean:
	rm -f etapa2
	rm -f lex.yy.*
	rm -f parser.tab.*
	rm -f report.out