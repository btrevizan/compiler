all: parser.y scanner.l main.c tree.c table.c stack.c checks.c
	bison -d parser.y --report-file=report.out -r all
	flex scanner.l
	gcc -c lex.yy.c parser.tab.c lexical.c tree.c table.c stack.c checks.c code.c iloc.c backpatching.c -g
	gcc -o etapa6 lex.yy.o parser.tab.o lexical.o tree.o table.o stack.o checks.o code.o iloc.o backpatching.o main.c -ll -lm

# test_analysis: etapa3 tests/e2/asl150
# 	./etapa3 < tests/e2/asl150
#
# test_leakage: etapa3 tests/e2/asl150
# 	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./etapa3 < tests/e2/asl150

clean:
	rm -f *.o
	rm -f parser.tab.*
	rm -f report.out
	rm -f lex.yy.*
	rm -f etapa6
	rm -f e3.csv
	rm -f leaks
