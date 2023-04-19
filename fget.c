/*
 * client.c -- TCP Socket Client
 *
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
#include <pwd.h>
#include <grp.h>
#define SIZE 1024

void processGetRequest(char *argv[], int socket_desc, int argc);
void processInfoRequest(char *argv[], int socket_desc);
void printFileProperties(struct stat stats);
void processMDRequest(char *argv[], int socket_desc);
void processPutRequest(char *argv[], int socket_desc, int argc);
void send_file(FILE *fp, int sockfd);
void processRmRequest(char *argv[], int socket_desc);
// void handleConnection(int argc, char *argv[]);
void handleConnection(void *arguments);

struct multiexecution_clientparams
{
	char** userInput;
    int argc;
};
// struct user_input
// {
// 	char *argv[];
//   int socket_desc;
//   int argc;
// };

/*
 * Main function 
 * @argument argc which indicates no of arguments
 * @argument argv which indicates the input arguments
 * @returns a int value
 */
int main(int argc, char *argv[])
{
  int socket_desc;
  struct sockaddr_in server_addr;
  char server_message[2000], client_message[2000];
  // pthread_t tid[2];
  // pthread_create(&(tid[0]), NULL, &pm_malloc, (void *)&mallocdata1);
	// pthread_join(tid[0], (void *)&address);

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
  // struct user_input  input;
	// mallocdata2.values = data1;
	// mallocdata2.size = 4;
  // while(1){
  if (strcmp(argv[1], "GET") == 0)
  {
    // pthread_create(&(tid[1]), NULL, &processGetRequest, (void *)&input);
	  // pthread_join(tid[1], (void *)&address);
   processGetRequest(argv, socket_desc, argc);
  }
  else if (strcmp(argv[1], "INFO") == 0)
  {
    processInfoRequest(argv, socket_desc);
  }
  else if (strcmp(argv[1], "MD") == 0)
  {
    processMDRequest(argv, socket_desc);
  }
  else if (strcmp(argv[1], "PUT") == 0)
  {

    processPutRequest(argv, socket_desc, argc);
  }
  else if (strcmp(argv[1], "RM") == 0)
  {
    processRmRequest(argv, socket_desc);
  }
 // }
  // Close the socket:
  close(socket_desc);

  return 0;
}

void handleConnection(void *arguments){
  struct multiexecution_clientparams *args = arguments;
  int argc = args->argc;
  char **argv = args->userInput;
  int socket_desc;
  struct sockaddr_in server_addr;
  char server_message[2000], client_message[2000];
  // pthread_t tid[2];
  // pthread_create(&(tid[0]), NULL, &pm_malloc, (void *)&mallocdata1);
	// pthread_join(tid[0], (void *)&address);

  // Clean buffers:
  memset(server_message, '\0', sizeof(server_message));
  memset(client_message, '\0', sizeof(client_message));

  // Create socket:
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_desc < 0)
  {
    printf("Unable to create socket\n");
    // return -1;
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
    // return -1;
  }
  printf("Connected with server successfully\n");
  // struct user_input  input;
	// mallocdata2.values = data1;
	// mallocdata2.size = 4;
  // while(1){
  if (strcmp(argv[1], "GET") == 0)
  {
    // pthread_create(&(tid[1]), NULL, &processGetRequest, (void *)&input);
	  // pthread_join(tid[1], (void *)&address);
   processGetRequest(argv, socket_desc, argc);
  }
  else if (strcmp(argv[1], "INFO") == 0)
  {
    processInfoRequest(argv, socket_desc);
  }
  else if (strcmp(argv[1], "MD") == 0)
  {
    processMDRequest(argv, socket_desc);
  }
  else if (strcmp(argv[1], "PUT") == 0)
  {

    processPutRequest(argv, socket_desc, argc);
  }
  else if (strcmp(argv[1], "RM") == 0)
  {
    processRmRequest(argv, socket_desc);
  }
 // }
  // Close the socket:
  close(socket_desc);
  
}

/*
 * Writes the data to a local file by getting the content from the server. 
 * @argument int type socket
 * @argument local path to which the data has to be saved.
 * @returns void
 */
void write_file(int sockfd, char *clientFilePath)
{
  int n;
  FILE *fp;
  char *filename = clientFilePath;
  char buffer[SIZE];

  fp = fopen(filename, "w");
  if (fp == NULL)
  {
    printf("File path does not exist. Please create the file path first.\n");
    perror("Error");
    exit(1);
  }
  // int sockfd1;
  while (1)
  {
    n = recv(sockfd, buffer, SIZE, 0);
    if (n <= 0)
    {
      printf("Check data save..\n");
      break;
    }
    printf("%s", buffer);
    fprintf(fp, "%s", buffer);
    bzero(buffer, SIZE);
  }
  fclose(fp);
  printf("test exit");
  return;
}

/*
 * Gets data to a local file by getting the content from the server. 
 * @argument input arguments by user
 * @argument int socket
 * @argument int number of arguments
 * @returns void
 */
void processGetRequest(char *argv[], int socket_desc, int argc)
{
  // char* server_filepath;
  FILE *fp;
  char *clientFilePath;

  if (argc == 3)
  {
    // printf("test get with local path mehtioned\n %s", argv);
    char* clientPathPrefix = "/Users/neo";
    // clientFilePath = argv[2];
    // strcat(clientPathPrefix, clientFilePath);

    clientFilePath = malloc(strlen(clientPathPrefix) + strlen(argv[2]) + 1);
     strcpy(clientFilePath, clientPathPrefix);
    strcat(clientFilePath, argv[2]);
    // printf("The client path using remote file path is: %s", clientFilePath);
  }
  else
  {
    clientFilePath = argv[3];
  }

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
  printf("Get operation completed..\n");
}

/*
 * Makes a directory in the server. 
 * @argument input arguments by user
 * @argument int socket
 * @returns void
 */
void processMDRequest(char *argv[], int socket_desc)
{
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
  }
  char server_message[2000];
  memset(server_message, '\0', sizeof(server_message));
  if (recv(socket_desc, server_message, sizeof(server_message), 0) < 0)
  {
    printf("Server message for folder creation: %s\n", server_message);
  }
}

/*
 * Get folder or file info from server 
 * @argument input arguments by user
 * @argument int socket
 * @returns void
 */
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

struct passwd *pw = getpwuid(stats.st_uid);
struct group  *gr = getgrgid(stats.st_gid);
// File owner
if (pw != NULL)
{
    printf("File owner: %s\n", pw->pw_name);
}
else
{
    printf("File owner name not found. \n");
}
// Group name
if (gr != NULL)
{
    printf("Group name: %s", gr->gr_name);
}
else
{
    printf("Group name not found.");
}

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
  printf("\nModified on: %d-%d-%d %d:%d:%d\n", dt.tm_mday, dt.tm_mon, dt.tm_year + 1900,
         dt.tm_hour, dt.tm_min, dt.tm_sec);
}

/*
 * Makes a directory in the server. 
 * @argument input arguments by user
 * @argument int socket
 * @returns void
 */
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

/*
 * Puts a file to the server from local disk. 
 * @argument input arguments by user
 * @argument int socket
 * @int number of arguments
 * @returns void
 */
void processPutRequest(char *argv[], int socket_desc, int argc)
{
  // char* clientMessage = processClientRequestInput(argv);
  int clientMessageLength = 0;
  char *folderPrefix;
  if (argc == 3)
  {
    folderPrefix = "/Volumes/USB1";
    clientMessageLength = strlen(argv[1]) + strlen(argv[2]) + 1 + strlen(folderPrefix);
    printf("Client message length is %d", strlen(folderPrefix));
  }
  else
  {
    clientMessageLength = strlen(argv[1]) + strlen(argv[3]) + 1;
  }

  // printf("the legth of inout %d \n",clientMessageLength );
  char client_message[clientMessageLength];
  if (argc == 3)
  {

    int i = 0;
    int k;

    for (i = 0; i < strlen(argv[1]); i++)
    {
      client_message[i] = argv[1][i];
    }

    client_message[i] = ' ';
    int j = i + 1;
    i = 0;
    for (k = j; k < strlen(folderPrefix) + j; k++)
    {
      client_message[k] = folderPrefix[i];
      i++;
    }
    j = k;
    i = 0;
    for (int k = j; k < strlen(argv[2]) + j; k++)
    {
      client_message[k] = argv[2][i];
      i++;
    }
    //  printf("Remote file path missing so updated path as per local path is %s", client_message);
  }
  else
  {
    int i = 0;
    for (i = 0; i < strlen(argv[1]); i++)
    {
      client_message[i] = argv[1][i];
    }

    client_message[i] = ' ';
    int j = i + 1;
    i = 0;
    for (int k = j; k < strlen(argv[3]) + j; k++)
    {
      client_message[k] = argv[3][i];
      i++;
    }
  }

  printf("Client message is: %s \n", client_message);
  if (send(socket_desc, client_message, clientMessageLength, 0) < 0)
  {
    printf("Unable to send message\n");
  }

  FILE *fp;
  char *filename = argv[2];
  fp = fopen(filename, "r");
  if (fp == NULL)
  {
    perror("[-]Error in reading file.");
    exit(1);
  }
  send_file(fp, socket_desc);
  printf("[+]File data sent successfully.\n");
}

/*
 * Sends file to the server from local disk. 
 * @argument file pointer
 * @argument int socket
 * @returns void
 */
void send_file(FILE *fp, int sockfd)
{
  int n;
  char data[SIZE] = {0};

  while (fgets(data, SIZE, fp) != NULL)
  {
    if (send(sockfd, data, sizeof(data), 0) == -1)
    {
      perror("[-]Error in sending file.");
      exit(1);
    }
    bzero(data, SIZE);
  }
}

/*
 * Removes a file or folder from remote system.
 * @argument input arguments by user
 * @argument int socket
 * @returns void
 */
void processRmRequest(char *argv[], int socket_desc)
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
  printf("Client message is: %s \n", client_message);
  if (send(socket_desc, client_message, clientMessageLength, 0) < 0)
  {
    printf("Unable to send message\n");
    // return -1;
  }

  char server_message[2000];
  memset(server_message, '\0', sizeof(server_message));
  if (recv(socket_desc, server_message, strlen(server_message), 0) < 0)
  {
    printf("Server message for folder creation: %s\n", server_message);
  }
}