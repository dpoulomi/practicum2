all: server fget multiclient

server: server.c
	gcc server.c -o server

client: fget.c
	gcc fget.c -o fget

multiclient: multiclient.c
	gcc -o multiclient multiclient.c 


# all: server client

# server: server.c
# 	gcc server.c -o server

# client: client.c
# 	gcc client.c -o client

# multiclient_execution: multiclient.c
# 	gcc multiclient.c -o multiclient


