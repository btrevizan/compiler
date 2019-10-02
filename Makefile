all: parser.y scanner.l main.c tree.c
	bison -d parser.y --report-file=report.out -r all
	flex scanner.l
	gcc -c lex.yy.c parser.tab.c tree.c
	gcc -o etapa3 lex.yy.o parser.tab.o tree.o main.c -ll

test_analysis: etapa3 tests/e2/asl150
	./etapa3 < tests/e2/asl150

test_leakage: etapa3 tests/e2/asl150
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./etapa3 < tests/e2/asl150

clean:
	rm -f *.o
	rm -f parser.tab.*
	rm -f report.out
	rm -f lex.yy.*
	rm -f etapa3
	rm -f e3.csv
	rm -f leaks
