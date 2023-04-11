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
#include <sys/stat.h>
#include <time.h>
#define SIZE 1024

void processGetRequest(char *argv[], int socket_desc);
void processInfoRequest(char *argv[], int socket_desc);
void printFileProperties(struct stat stats);
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
  else if (strcmp(argv[1], "INFO") == 0)
  {
    processInfoRequest(argv, socket_desc);
  }

  // else if(strcmp(argv[1], "MD") == 0){
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
  printf("%s \n", client_message);
  if (send(socket_desc, client_message, clientMessageLength, 0) < 0)
  {
    printf("Unable to send message\n");
    // return -1;
  }
  write_file(socket_desc, clientFilePath);
}

void processInfoRequest(char *argv[], int socket_desc)
{
  struct stat stats;
  // char* clientMessage = processClientRequestInput(argv);
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
  printf("Client message is: %s \n", client_message);
  if (send(socket_desc, client_message, clientMessageLength, 0) < 0)
  {
    printf("Unable to send message\n");
    // return -1;
  }

  recv(socket_desc, &stats, SIZE, 0);
  printFileProperties(stats);
}

/**
 * Function to print file properties.
 */
void printFileProperties(struct stat stats)
{
  struct tm dt;

  // File permissions
  printf("\nFile access: ");
  if (stats.st_mode & R_OK)
    printf("read ");
  if (stats.st_mode & W_OK)
    printf("write ");
  if (stats.st_mode & X_OK)
    printf("execute");

  // File size
  printf("\nFile size: %d", stats.st_size);

  // Get file creation time in seconds and
  // convert seconds to date and time format
  dt = *(gmtime(&stats.st_ctime));
  printf("\nCreated on: %d-%d-%d %d:%d:%d", dt.tm_mday, dt.tm_mon, dt.tm_year + 1900,
         dt.tm_hour, dt.tm_min, dt.tm_sec);

  // File modification time
  dt = *(gmtime(&stats.st_mtime));
  printf("\nModified on: %d-%d-%d %d:%d:%d", dt.tm_mday, dt.tm_mon, dt.tm_year + 1900,
         dt.tm_hour, dt.tm_min, dt.tm_sec);
}

char *processClientRequestInput(char *argv[])
{
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
  printf("%s \n", client_message);
  return client_message;
}
