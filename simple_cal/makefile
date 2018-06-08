calc: cal.y cal.l
	bison -d cal.y
	flex cal.l
	gcc -o $@ cal.tab.c lex.yy.c -lfl

.PHONY: clean
clean:
	rm -f calc *yy.c *.tab.*

