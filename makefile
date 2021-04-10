CFLAGS = -g -Wall -Wextra -Werror -ansi -pedantic

%: %.c
	gcc $(CFLAGS) -o $@ $^
