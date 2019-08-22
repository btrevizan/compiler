all: scanner.l tokens.h main.c
    flex scanner.l
    gcc -o etapa1 lex.yy.c main.c

test: test.txt etapa1
    cat test.txt | ./etapa1 > output.txt
	diff output.txt right_output.txt

clean:
    rm -f etapa1
    rm -f lex.yy.c
    rm output.txt