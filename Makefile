build:2048.c
	gcc 2048.c -o 2048 -lncurses
run:
	./2048
clean:
	rm -rf 2048
