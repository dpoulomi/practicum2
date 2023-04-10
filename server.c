/*
 * server.c -- TCP Socket Server
 *
 * adapted from:
 *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#define SIZE 1024

void processClientRequest(int client_sock, char client_message[8196], int socket_desc);
void processGetRequest(char *dir, int sockfd);
void send_file(FILE *fp, int sockfd);
int main(void)
{
  int socket_desc, client_sock;
  socklen_t client_size;
  struct sockaddr_in server_addr, client_addr;
  char server_message[8196], client_message[8196];

  // Clean buffers:
  memset(server_message, '\0', sizeof(server_message));
  memset(client_message, '\0', sizeof(client_message));

  // Create socket:
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_desc < 0)
  {
    printf("Error while creating socket\n");
    return -1;
  }
  printf("Socket created successfully\n");

  // Set port and IP:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(2000);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // Bind to the set port and IP:
  if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    printf("Couldn't bind to the port\n");
    return -1;
  }
  printf("Done with binding\n");

  // Listen for clients:
  if (listen(socket_desc, 1) < 0)
  {
    printf("Error while listening\n");
    return -1;
  }
  printf("\nListening for incoming connections.....\n");

  // Accept an incoming connection:
  client_size = sizeof(client_addr);
  client_sock = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);

  if (client_sock < 0)
  {
    printf("Can't accept\n");
    return -1;
  }
  printf("Client connected at IP: %s and port: %i\n",
         inet_ntoa(client_addr.sin_addr),
         ntohs(client_addr.sin_port));
  // printf("%s", argv[1], argv[2]);

  processClientRequest(client_sock, client_message,socket_desc);
  // // Receive client's message:
  // if (recv(client_sock, client_message,
  //          sizeof(client_message), 0) < 0){
  //   printf("Couldn't receive\n");
  //   return -1;
  // }
  // printf("Msg from client: %s\n", client_message);

  // // Respond to client:
  // strcpy(server_message, "This is the server's response message.");

  // if (send(client_sock, server_message, strlen(server_message), 0) < 0){
  //   printf("Can't send\n");
  //   return -1;
  // }
  // Questin 1: fetch a file data
  // if (recv(client_sock, client_message,
  //          sizeof(client_message), 0) < 0){
  //   printf("Couldn't receive\n");
  //   return -1;
  // }

  // printf("Msg from client: %s\n", client_message);

  // // Respond to client with :
  // strcpy(server_message, "This is the server's response message.");

  // if (send(client_sock, server_message, strlen(server_message), 0) < 0){
  //   printf("Can't send\n");
  //   return -1;
  // }
  // Closing the socket:
  close(client_sock);
  close(socket_desc);

  return 0;
}
void processClientRequest(int client_sock, char client_message[8196], int socket_desc)
{

  // Receive client's message:
  char server_message[8196];

  if (recv(client_sock, client_message,
           100, 0) < 0)
  {
    printf("Couldn't receive\n");
    // return -1;
  }
  printf("the legth of inout %d \n", strlen(client_message));
  printf("Msg from client: %s\n", client_message);

  char *ptr = strtok(client_message, " ");
  char *array[2];
  int i = 0;
  while (ptr != NULL)
  {
    array[i++] = ptr;
    ptr = strtok(NULL, " ");
  }
  // printf("ptr 0 is: %s\n", ptr[0]);
  // printf("ptr 1 is: %s\n", ptr[1]);
  // while (ptr != NULL)
  // {
  // printf("'%s'\n", ptr);
  if (strcmp(array[0], "GET") == 0)
  {
    processGetRequest(array[1], socket_desc);
  }
  // else if (strcmp(ptr[0], "INFO") == 0)
  // {
  //   processInfoRequest();
  // }
  // else if (strcmp(ptr[0], "MD") == 0)
  // {
  //   processMdRequest();
  // }
  // else if (strcmp(ptr[0], "PUT") == 0)
  // {
  //   processPutRequest();
  // }
  // else if (strcmp(ptr[0], "RM") == 0)
  // {
  //   processRmRequest();
  // }

  // ptr = strtok(NULL, " ");
  // }

  // Respond to client:

  strcpy(server_message, "This is the server's response message.");

  if (send(client_sock, server_message, strlen(server_message), 0) < 0)
  {
    printf("Can't send\n");
    // return -1;
  }
}
char *remove_end(char *str, char c)
{
  char *last_pos = strrchr(str, c);

  if (last_pos != NULL)
  {
    *last_pos = '\0';
    return last_pos + 1; /* pointer to the removed part of the string */
  }

  return NULL; /* c wasn't found in the string */
}

void processGetRequest(char *path, int sockfd)
{
  // char * token, * last;
  char* filePath = path;
  // last = token = strtok(path, "/");
  // for (;(token = strtok(NULL, "/")) != NULL; last = token);
  char *removed;
  // printf("%s\n", path);
  removed = remove_end(filePath, '/');
  printf("%s\n", removed);
  printf("%s\n", path);
   printf("%s\n", filePath);
  DIR *d;
  struct dirent *dir;
  FILE *fp;

  d = opendir(filePath);
  if (d)
  {
    while ((dir = readdir(d)) != NULL)
    {
      if (dir->d_type == DT_REG && strcmp(removed, dir->d_name) == 0)
      {
        printf("%s\n", dir->d_name);
        fp = fopen(path, "r");
        if (fp == NULL)
        {
          perror("[-]Error in reading file.");
          exit(1);
        }

        send_file(fp, sockfd);
        printf("[+]File data sent successfully.\n");

        printf("[+]Closing the connection.\n");
      }
    }
    closedir(d);
  }
  // return (0);
}

void send_file(FILE *fp, int sockfd){
  int n;
  char data[SIZE] = {0};
 
  while(fgets(data, SIZE, fp) != NULL) {
    if (send(sockfd, data, sizeof(data), 0) == -1) {
      perror("[-]Error in sending file.");
      exit(1);
    }
    bzero(data, SIZE);
  }
}
