calc: calc.y calc.l calc.h funcs.c
	bison -d calc.y
	flex calc.l
	gcc -o $@ funcs.c calc.tab.c lex.yy.c -lfl


.PHONY: clean
clean:
	rm calc lex.yy.c calc.tab.c calc.tab.h
