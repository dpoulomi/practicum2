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
#define SIZE 1024

void processGetRequest(char* argv[], int socket_desc );
int main(int argc, char* argv[])
{
  int socket_desc;
  struct sockaddr_in server_addr;
  char server_message[2000], client_message[2000];
  
  // Clean buffers:
  memset(server_message,'\0',sizeof(server_message));
  memset(client_message,'\0',sizeof(client_message));
  
  // Create socket:
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  
  if(socket_desc < 0){
    printf("Unable to create socket\n");
    return -1;
  }
  
  printf("Socket created successfully\n");
  
  // Set port and IP the same as server-side:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(2000);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  
  // Send connection request to server:
  if(connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
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




char* operation = argv[1];
if (strcmp(argv[1], "GET") == 0){
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


  //allow you to send a command to the server for reading a file from the server's file storage area
  // printf("Enter client command to send a command to the server for reading a file from the server's file storage area: \n");
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

void write_file(int sockfd, char * clientFilePath){
  int n;
  FILE *fp;
  char *filename = clientFilePath;
  char buffer[SIZE];
  printf("File path: %s\n", clientFilePath);
  fp = fopen(filename, "w");
  while (1) {
    n = recv(sockfd, buffer, SIZE, 0);
    if (n <= 0){
      break;
      return;
    }
    fprintf(fp, "%s", buffer);
    bzero(buffer, SIZE);
  }
  return;
}

void processGetRequest(char* argv[], int socket_desc ){
  // char* server_filepath;
  FILE *fp;
  char *clientFilePath = argv[2];
  int clientMessageLength = strlen(argv[1]) + strlen(argv[2]) + 1;

  // printf("the legth of inout %d \n",clientMessageLength );
  char client_message[clientMessageLength] ;
  int i = 0;
  for (i = 0; i < strlen(argv[1]) ; i++){
    client_message[i] = argv[1][i];
  }
   printf("%s \n", client_message);
   client_message[i] = ' ';
  int j = i + 1;
  i = 0;
  for (int k = j; k < strlen(argv[2]) + j ; k++){
    client_message[k] = argv[2][i];
    i++;
  }
  char server_message[2000];
  memset(server_message,'\0',sizeof(server_message));
   printf("%s \n", client_message);
  if(send(socket_desc, client_message, clientMessageLength, 0) < 0){
    printf("Unable to send message\n");
    //return -1;
  }
  if(recv(socket_desc, server_message, sizeof(server_message), 0) < 0){
    printf("Error while receiving server's msg\n");
    //return -1;
  }
  write_file(socket_desc, clientFilePath);
  printf("Server's response: %s\n",server_message);


}
