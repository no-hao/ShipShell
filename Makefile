wish: shell.o parser.o command_handler.o error_handler.o
	gcc -o wish shell.o parser.o command_handler.o error_handler.o

shell.o: shell.c command_handler.h parser.h error_handler.h
	gcc -c shell.c

parser.o: parser.c parser.h
	gcc -c parser.c

command_handler.o: command_handler.c command_handler.h
	gcc -c command_handler.c

error_handler.o: error_handler.c error_handler.h
	gcc -c error_handler.c

clean:
	rm -f *.o wish

