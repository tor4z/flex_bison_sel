inc: include.l
	flex include.l
	gcc -o $@ lex.yy.c


.PHONY: clean
clean:
	rm -f inc *.yy.c 
