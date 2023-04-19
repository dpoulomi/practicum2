all: server fget

server: server.c
	gcc server.c -o server

client: fget.c
	gcc fget.c -o fget

multiclient_execution: multiclient.c
	gcc multiclient.c -o multiclient

