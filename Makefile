wish: main.o parser.o external_cmds.o debug_and_errors.o errors.o path_mgmt.o builtin_cmds.o shell_operations.o
	gcc -g -o wish main.o parser.o external_cmds.o debug_and_errors.o path_mgmt.o builtin_cmds.o shell_operations.o

main.o: main.c external_cmds.h parser.h debug_and_errors.h
	gcc -c main.c

parser.o: parser.c parser.h debug_and_errors.h
	gcc -c parser.c

external_cmds.o: external_cmds.c external_cmds.h debug_and_errors.h
	gcc -c external_cmds.c

builtin_cmds: builtin_cmds.h debug_and_errors.h
	gcc -c builtin_cmds.c

shell_operations: shell_operations.h
	gcc -c shell_operations.c

debug_and_errors.o: debug_and_errors.c debug_and_errors.h
	gcc -c debug_and_errors.c

clean:
	rm -f *.o wish
