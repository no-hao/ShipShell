wish: shell.o parser.o external_cmds.o util.o errors.o
	gcc -o wish shell.o parser.o external_cmds.o util.o errors.o

shell.o: shell.c external_cmds.h parser.h util.h errors.h
	gcc -c shell.c

parser.o: parser.c parser.h util.h errors.h
	gcc -c parser.c

external_cmds.o: external_cmds.c external_cmds.h util.h errors.h
	gcc -c external_cmds.c

util.o: util.c util.h errors.h
	gcc -c util.c

errors.o: errors.c errors.h
	gcc -c errors.c

clean:
	rm -f *.o wish
