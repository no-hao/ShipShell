wish: main.o parser.o external_cmds.o debug_and_errors.o errors.o path_mgmt.o builtin_cmds.o redirection.o parallel_processing.o
	gcc -g -o wish main.o parser.o external_cmds.o debug_and_errors.o path_mgmt.o builtin_cmds.o redirection.o parallel_processing.o

main.o: main.c external_cmds.h parser.h debug_and_errors.h
	gcc -c main.c

parser.o: parser.c parser.h debug_and_errors.h
	gcc -c parser.c

external_cmds.o: external_cmds.c external_cmds.h debug_and_errors.h
	gcc -c external_cmds.c

builtin_cmds: builtin_cmds.h debug_and_errors.h
	gcc -c builtin_cmds.c

redirection: redirection.h
	gcc -c redirection.c

parallel_processing: parallel_processing.h
	gcc -c parallel_processing.c

debug_and_errors.o: debug_and_errors.c debug_and_errors.h
	gcc -c debug_and_errors.c

clean:
	rm -f *.o wish
