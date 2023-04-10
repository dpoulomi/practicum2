/*
 * client.c -- TCP Socket Client
 *
 * adapted from:
 *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#define SIZE 1024

void processGetRequest(char *argv[], int socket_desc);
int main(int argc, char *argv[])
{
  int socket_desc;
  struct sockaddr_in server_addr;
  char server_message[2000], client_message[2000];

  // Clean buffers:
  memset(server_message, '\0', sizeof(server_message));
  memset(client_message, '\0', sizeof(client_message));

  // Create socket:
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_desc < 0)
  {
    printf("Unable to create socket\n");
    return -1;
  }

  printf("Socket created successfully\n");

  // Set port and IP the same as server-side:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(2000);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // Send connection request to server:
  if (connect(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    printf("Unable to connect\n");
    return -1;
  }
  printf("Connected with server successfully\n");

  // Get input from the user:
  // printf("Enter message: ");
  // gets(client_message);

  // // Send the message to server:
  // if(send(socket_desc, client_message, strlen(client_message), 0) < 0){
  //   printf("Unable to send message\n");
  //   return -1;
  // }

  // // Receive the server's response:
  // if(recv(socket_desc, server_message, sizeof(server_message), 0) < 0){
  //   printf("Error while receiving server's msg\n");
  //   return -1;
  // }

  // printf("Server's response: %s\n",server_message);

  char *operation = argv[1];
  if (strcmp(argv[1], "GET") == 0)
  {
    processGetRequest(argv, socket_desc);
  }
  // else if(strcmp(argv[1], "INFO") == 0){
  //     processInfoRequest();
  // }else if(strcmp(argv[1], "MD") == 0){
  //     processMdRequest();
  // }else if(strcmp(argv[1], "PUT") == 0){
  //     processPutRequest();
  // }else if(strcmp(argv[1], "RM") == 0){
  //     processRmRequest();
  // }

  // allow you to send a command to the server for reading a file from the server's file storage area
  //  printf("Enter client command to send a command to the server for reading a file from the server's file storage area: \n");
  //  //% fget GET folder/foo.txt data/localfoo.txt
  //  gets(client_message);
  //  if(send(socket_desc, client_message, strlen(client_message), 0) < 0){
  //    printf("Unable to send message\n");
  //    return -1;
  //  }
  //  if(recv(socket_desc, server_message, sizeof(server_message), 0) < 0){
  //    printf("Error while receiving server's msg\n");
  //    return -1;
  //  }
  //  printf("Server's response: %s\n",server_message);

  // //a command that gets all file information about a file,such as ownership, data of last modification, permissions, size, etc
  //  printf("Enter client command to get file information about reading a file from the server's file storage area: \n");
  // //% fget GET folder/foo.txt data/localfoo.txt
  // gets(client_message);
  // if(send(socket_desc, client_message, strlen(client_message), 0) < 0){
  //   printf("Unable to send message\n");
  //   return -1;
  // }
  // if(recv(socket_desc, server_message, sizeof(server_message), 0) < 0){
  //   printf("Error while receiving server's msg\n");
  //   return -1;
  // }
  // printf("Server's response: %s\n",server_message);

  // Close the socket:
  close(socket_desc);

  return 0;
}

void write_file(int sockfd, char *clientFilePath)
{
  int n;
  FILE *fp;
  char *filename = clientFilePath;
  char buffer[SIZE];
  // printf("File path: %s\n", clientFilePath);
  
  fp = fopen(filename, "w");
  while (1)
  {
    printf("Check data save..\n");
    n = recv(sockfd, buffer, SIZE, 0);
    if (n <= 0)
    {
      break;
      return;
    }
    fprintf(fp, "%s", buffer);
    bzero(buffer, SIZE);
  }
  return;
}

void processGetRequest(char *argv[], int socket_desc)
{
  // char* server_filepath;
  FILE *fp;
  char *clientFilePath = argv[3];
  printf("Client file path is %s \n", clientFilePath);
  int clientMessageLength = strlen(argv[1]) + strlen(argv[2]) + 1;

  // printf("the legth of inout %d \n",clientMessageLength );
  char client_message[clientMessageLength];
  int i = 0;
  for (i = 0; i < strlen(argv[1]); i++)
  {
    client_message[i] = argv[1][i];
  }

  client_message[i] = ' ';
  int j = i + 1;
  i = 0;
  for (int k = j; k < strlen(argv[2]) + j; k++)
  {
    client_message[k] = argv[2][i];
    i++;
  }
  // char server_message[2000];
  // memset(server_message,'\0',sizeof(server_message));
  printf("%s \n", client_message);
  if (send(socket_desc, client_message, clientMessageLength, 0) < 0)
  {
    printf("Unable to send message\n");
    // return -1;
  }
  // if (recv(client_socket, buffer, BUFSIZ, 0) < 0)
  //   {
  //     printf("Error while receiving server's msg\n");

  //   }
  // file_size = atoi(buffer);

  // if(recv(socket_desc, server_message, sizeof(server_message), 0) < 0){
  //   printf("Error while receiving server's msg\n");
  //   //return -1;
  // }
  write_file(socket_desc, clientFilePath);
  /* Receiving file size */
  // char buffer[BUFSIZ];
  // int file_size;
  // FILE *received_file;
  // int remain_data = 0;
  // recv(socket_desc, buffer, BUFSIZ, 0);
  // file_size = atoi(buffer);
  // ssize_t len;
  // // fprintf(stdout, "\nFile size : %d\n", file_size);
  
  // received_file = fopen(clientFilePath, "a+");
  // if (received_file == NULL)
  // {
  //   fprintf(stderr, "Failed to open file foo --> %s\n", strerror(errno));

  //   exit(EXIT_FAILURE);
  // }

  // remain_data = file_size;

  // while ((remain_data > 0) && ((len = recv(socket_desc, buffer, BUFSIZ, 0)) > 0))
  // {
  //   printf("Check data save..\n");
  //   fwrite(buffer, sizeof(char), len, received_file);
  //   remain_data -= len;
  //   printf(stdout, "Receive %d bytes and we hope :- %d bytes\n", len, remain_data);
  //   fprintf(stdout, "Receive %d bytes and we hope :- %d bytes\n", len, remain_data);
  // }
  // fclose(received_file);
  // printf("Server's response: %s\n", server_message);
}
