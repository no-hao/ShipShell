wish: shell.o parser.o command_handler.o util.o errors.o
	gcc -o wish shell.o parser.o command_handler.o util.o errors.o

shell.o: shell.c command_handler.h parser.h util.h errors.h
	gcc -c shell.c

parser.o: parser.c parser.h util.h errors.h
	gcc -c parser.c

command_handler.o: command_handler.c command_handler.h util.h errors.h
	gcc -c command_handler.c

util.o: util.c util.h errors.h
	gcc -c util.c

errors.o: errors.c errors.h
	gcc -c errors.c

clean:
	rm -f *.o wish
