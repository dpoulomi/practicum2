all: server fget test

server: server.c
	gcc server.c -o server

client: fget.c
	gcc fget.c -o fget

test:test.c
	gcc test.c -o test