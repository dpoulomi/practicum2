all: server fget test

server: server.c
	gcc server.c -o server

client: fget.c
	gcc fget.c -o fget

multiclient_execution: multiclient_execution.c
	gcc multiclient_execution.c -o multiclient_execution

