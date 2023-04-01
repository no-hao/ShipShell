wish: main.o
	gcc -g -o wish main.o

main.o: main.c
	gcc -c main.c

clean:
	rm -f *.o wish
