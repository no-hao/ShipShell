wish: shell.o parser.o command_handler.o
	gcc -o wish shell.o parser.o command_handler.o

shell.o: shell.c command_handler.h parser.h
	gcc -c shell.c

parser.o: parser.c parser.h
	gcc -c parser.c

command_handler.o: command_handler.c command_handler.h
	gcc -c command_handler.c

clean:
	rm -f *.o wish

