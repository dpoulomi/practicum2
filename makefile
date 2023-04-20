all: fserver fget multiclient

server: fserver.c
	gcc fserver.c -o fserver

client: fget.c
	gcc fget.c -o fget

multiclient: multiclient.c
	gcc -o multiclient multiclient.c 



