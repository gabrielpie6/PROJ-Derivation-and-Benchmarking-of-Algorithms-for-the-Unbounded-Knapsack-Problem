PROJ_NAME=ukp

CC=gcc
CFLAGS=-ggdb -O0 -std=c99 -fstack-protector-all -fstack-usage -Werror=implicit-function-declaration -D_XOPEN_SOURCE=700
LDFLAGS=-O0 -lm

$(PROJ_NAME): *.c *.h
	$(CC) $(CFLAGS) *.c -I. -o $(PROJ_NAME) $(LDFLAGS)