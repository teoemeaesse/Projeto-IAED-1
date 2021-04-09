CFLAGS = -g -ansi -pedantic -Wall

%: %.c
	gcc $(CFLAGS) -o $@ $^
