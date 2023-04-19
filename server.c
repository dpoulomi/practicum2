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
#include <ftw.h>
#include <fcntl.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>

#define TIME_TO_WAIT 5
#define SIZE 1024
#define USB1 "/Volumes/USB1/"
#define USB2 "/Volumes/USB2/"

// clock_t last = clock();
void processClientRequest(int client_sock, char client_message[8196], int socket_desc);
void processGetRequest(char *dir, int sockfd);
void processInfoRequest(char *path, int sockfd);
void send_file(FILE *fp, int sockfd);
void processMDRequest(char *path, int sockfd);
void processPutRequest(char *path, int sockfd);
char *processRequestForOtherDevice(char *path);
void processRmRequest(char *path, int sockfd);
int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);
void copyFileToOtherUSB(char *path, char *secondPath);
void checkServerAvailability();
int rmrf(char *path);
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

struct Server_filesystem
{
  // char* USB1 = "USB1";
  // char* USB2 = "USB2";
  int USB1Available;
  int USB2Available;
  int syncUSB1;
  int syncUSB2;
};
struct Server_filesystem serverInfo;

/*
 * Main function
 * @argument void
 * @returns a int value
 */
int main(void)
{
  int socket_desc, client_sock;
  socklen_t client_size;
  struct sockaddr_in server_addr, client_addr;
  char server_message[8196], client_message[8196];
  pthread_t tid;
  pid_t childpid;
  // struct Server_filesystem serverInfo;
  serverInfo.USB1Available = 0;
  serverInfo.USB2Available = 0;
  serverInfo.syncUSB1 = 0;
  serverInfo.syncUSB2 = 0;
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
  if (listen(socket_desc, 2) < 0)
  {
    printf("Error while listening\n");
    return -1;
  }

  // Listen for clients:
  clock_t last = clock();
  while (1)
  {
    // pthread_t tid;
    // pthread_create(&tid, NULL, &threadproc, (* void)  &serverInfo);
    clock_t current = clock();
    // if (current >= (last + TIME_TO_WAIT * CLOCKS_PER_SEC))
    if (current >= (last + TIME_TO_WAIT))
    {
      checkServerAvailability(); /* insert your function here */
      last = current;
    }
    printf("\nListening for incoming connections.....\n");
    memset(client_message, '\0', sizeof(client_message));
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

    // if ((childpid = fork()) == 0)
    // {
    //   close(socket_desc);
    //   while (1)
    //   {
    processClientRequest(client_sock, client_message, socket_desc);
    close(client_sock);
  }
  //   }
  // }
  close(socket_desc);

  return 0;
}

// void *threadproc(void *arg)
// {
//   struct Server_filesystem serverInfo;
//     while()
//     {
//         sleep(20);
//         checkServerAvailability();
//     }
//     return 0;
// }

void checkServerAvailability()
{
  pthread_mutex_lock(&lock);
  printf("Server syncing method called..\n");
  //
  struct stat stats;
  if (stat(USB1, &stats) == -1)
  {
    printf("USB1 is missing.\n");
    serverInfo.USB1Available = 1;
    serverInfo.syncUSB1 = 1;
  }
  else
  {
    printf("USB1 is available.\n");
    serverInfo.USB1Available = 0;
    // serverInfo.synced = 0;
  }
  if (stat(USB2, &stats) == -1)
  {
    printf("USB2 is missing.\n");
    serverInfo.USB2Available = 1;
    serverInfo.syncUSB2 = 1;
  }
  else
  {
    printf("USB2 is available.\n");
    serverInfo.USB2Available = 0;
    // serverInfo.synced = 0;
  }
  if (serverInfo.USB1Available == 0 && serverInfo.USB2Available == 0)
  {
    if (serverInfo.syncUSB2 == 1)
    {
      printf("USB2 to be synced.\n");
      system("rsync -avu " USB1 " " USB2);
      serverInfo.syncUSB2 = 0;
    }
    else if (serverInfo.syncUSB1 == 1)
    {
      printf("USB1 to be synced.\n");
      system("rsync -avu " USB2 " " USB1);
      serverInfo.syncUSB1 = 0;
    }
  }
  // else if (!serverInfo.USB1Available && serverInfo.USB2Available)
  // {
  //   if (!serverInfo.synced)
  //   {
  //     system("rsync -avu " USB2 " " USB1);
  //   }
  // }
  pthread_mutex_unlock(&lock);
}

/*
 * Takes in input arguments from the client and redirects to appropriate method functionality.
 * @argument int socket
 * @argument formatted client message
 * @argument int client socket
 * @returns void
 */
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
  printf("array 0: %s\n", array[0]);
  printf("array 1: %s\n", array[1]);
  char *pathPrefix = "/Volumes/USB1/";
  // clientFilePath = argv[2];
  // strcat(clientPathPrefix, clientFilePath);

  char *filePath = malloc(strlen(pathPrefix) + strlen(array[1]) + 1);
  strcpy(filePath, pathPrefix);
  strcat(filePath, array[1]);
  if (strcmp(array[0], "GET") == 0)
  {
    processGetRequest(filePath, client_sock);
  }
  else if (strcmp(array[0], "INFO") == 0)
  {
    printf("Entering info block");
    processInfoRequest(filePath, client_sock);
  }
  else if (strcmp(array[0], "MD") == 0)
  {
    processMDRequest(filePath, client_sock);
  }
  else if (strcmp(array[0], "PUT") == 0)
  {
    processPutRequest(filePath, client_sock);
  }
  else if (strcmp(array[0], "RM") == 0)
  {
    processRmRequest(filePath, client_sock);
  }

  // ptr = strtok(NULL, " ");
  // }

  // Respond to client:

  // strcpy(server_message, "This is the server's response message.");

  // if (send(client_sock, server_message, strlen(server_message), 0) < 0)
  // {
  //   printf("Can't send\n");
  //   // return -1;
  // }
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

/*
 * Gets data to a local file by getting the content from the server.
 * @argument path from where the data needs to be fetched
 * @argument int socket
 * @returns void
 */
void processGetRequest(char *path, int sockfd)
{
  pthread_mutex_lock(&lock);
  FILE *fp;
  fp = fopen(path, "r");
  char *otherDevicePath;
  if (fp == NULL)
  {
    char *otherDevicePath = processRequestForOtherDevice(path);
    fp = fopen(otherDevicePath, "r");
    printf("Fetching data from other device at path %s\n", otherDevicePath);
    if (fp == NULL)
    {
      perror("[-]Error in reading file.");
      exit(1);
    }
  }
  send_file(fp, sockfd);
  printf("[+]File data sent successfully.\n");
  pthread_mutex_unlock(&lock);
  // printf("[+]Closing the connection.\n");
}

/*
 * Sends file to the client.
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
  //  printf("test exit");
}

/*
 * Send folder or file info to client
 * @argument input arguments by user
 * @argument int socket
 * @returns void
 */
void processInfoRequest(char *path, int sockfd)
{
  pthread_mutex_lock(&lock);
  // printf("ENtered in info method\n");
  struct stat stats;
  if (stat(path, &stats) == 0)
  {
    if (send(sockfd, &stats, sizeof(stats), 0) == -1)
    {
      perror("[-]Error in sending info.");
      exit(1);
    }
  }
  else
  {
    char *otherDevicePath = processRequestForOtherDevice(path);
    if (stat(path, &stats) == 0)
    {
      if (send(sockfd, &stats, sizeof(stats), 0) == -1)
      {
        perror("[-]Error in sending info.");
        exit(1);
      }
    }
    else
    {
      printf("Unable to get file properties.\n");
      printf("Please check whether '%s' file exists.\n", path);
    }
  }
  pthread_mutex_unlock(&lock);
}

/*
 * Makes a directory in the server.
 * @argument input arguments by user
 * @argument int socket
 * @returns void
 */
void processMDRequest(char *path, int sockfd)
{
  pthread_mutex_lock(&lock);
  printf("Entered in MD method\n");
  int check1, check2;
  char *dirname = path;
  char *otherDevicePath = processRequestForOtherDevice(path);
  char server_message[2000];
  memset(server_message, '\0', sizeof(server_message));
  check1 = mkdir(dirname, 0777);
  check2 = mkdir(otherDevicePath, 0777);
  // check if directory is created or not
  if (!check1)
  {
    strcpy(server_message, "Directory created in USB1.");
    printf("Directory created in USB1\n");
  }
  else
  {
    strcpy(server_message, "Unable to create directory in USB1.");
    printf("Unable to create directory in USB1\n");
    exit(1);
  }

  if (!check2)
  {
    strcpy(server_message, "Directory created in USB2.");
    printf("Directory created in USB2\n");
  }
  else
  {
    strcpy(server_message, "Unable to create directory in USB2.");
    printf("Unable to create directory in USB2\n");
    exit(1);
  }

  if (send(sockfd, server_message, sizeof(server_message), 0) < 0)
  {
    printf("Can't send\n");
  }
  pthread_mutex_unlock(&lock);
}

/*
 * Puts a file to the server from local disk.
 * @argument input arguments by user
 * @argument int socket
 * @returns void
 */
void processPutRequest(char *path, int sockfd)
{
  pthread_mutex_lock(&lock);
  FILE *fp;
  char *filename = path;
  printf("first filename is %s and length: %d\n", filename, strlen(filename));

  // for (int i = 0; i < strlen(filename); i++)
  // {
  //   printf("The characetrs are %c\n", filename[i]);
  // }

  char buffer[SIZE];

  fp = fopen(filename, "w");
  if (fp == NULL)
  {
    printf("First file path does not exist. Please create the file path first.\n");
    // perror("Error");
    // exit(1);
  }
  printf("filename :%s \n", filename);
  char *secondPath = processRequestForOtherDevice(path);
  printf("second filename is %s and length: %d\n", secondPath, strlen(secondPath));

  int n;

  FILE *fp1;
  // for(int i = 0 ; i<strlen(secondPath);i++ ){
  //   printf("The characetrs are %c\n", secondPath[i]);
  // }
  // printf("The comparison is : %d",  strcmp(secondPath, "/Volumes/USB2/test_folder4/test_server_put173.c"));

  //  int check;
  //  char * folder_path =  remove_end(secondPath, '/');
  //    check = mkdir(secondPath, 0777);
  fp1 = fopen(secondPath, "w"); // "/Volumes/USB2/test_folder4/test_server_put172.c"
  if (fp1 == NULL)
  {
    printf("Second file path does not exist. Please create the file path first.\n");
    // perror("Error");
    // exit(1);
  }
  // check if directory is created or not
  // if (!check)
  // {

  //   printf("Mirrored directory created\n");
  // }
  // else
  // {

  //   printf("Unable to create mirrored directory\n");
  //   exit(1);
  // }
  while (1)
  {
    n = recv(sockfd, buffer, SIZE, 0);
    if (n <= 0)
    {
      break;
    }
    // printf("First Buffer: %s\n", buffer);
    if (fp != NULL)
    {
      fprintf(fp, "%s", buffer);
    }
    // printf("Second Buffer: %s\n", buffer);
    if (fp1 != NULL)
    {
      fprintf(fp1, "%s", buffer);
    }
    //  printf( "Buffer is %s", buffer);

    bzero(buffer, SIZE);
  }
  if (fp != NULL)
  {
    fclose(fp);
  }
  if (fp1 != NULL)
  {
    fclose(fp1);
  }
  // char *secondPath = processPutRequestForOtherDevice(path, sockfd);
  // copyFileToOtherUSB(path, secondPath);
  pthread_mutex_unlock(&lock);
  return;
}

void copyFileToOtherUSB(char *path, char *secondPath)
{

  FILE *fptr1, *fptr2;
  char ch, fname1[20], fname2[20];
  printf("first filename is %s and length: %d\n", path, strlen(path));
  printf("second filename is %s and length: %d\n", secondPath, strlen(secondPath));
  // printf("\n Program to copy a file in another name: \n");
  // printf("Enter the source file name: ");
  // scanf("%s", fname1);

  fptr1 = fopen(path, "r");
  if (fptr1 == NULL)
  {
    printf("File1 does not found or an error occured when opening!!");
    perror("Error");
    exit(1);
  }
  // printf("\n Enter the new file name: ");
  // scanf("%s", secondPath);
  fptr2 = fopen(secondPath, "w");
  if (fptr2 == NULL)
  {
    printf("File2 does not found or an error occured when opening!!");
    perror("Error");
    fclose(fptr1);
    exit(2);
  }
  while (1)
  {
    ch = fgetc(fptr1);

    if (ch == EOF)
    {
      break;
    }
    else
    {
      fputc(ch, fptr2);
    }
  }
  printf("The file %s copied to file %s succesfully.\n", path, secondPath);
  fclose(fptr1);
  fclose(fptr2);
}

/*
 * Creates path for the altenative USB device for mirroring
 * @argument original string path provided by the user
 * @returns the alternate path of the other USB device
 */
char *processRequestForOtherDevice(char *path)
{
  // char string[100];
  char *secondPath = malloc(strlen(path));
  // char secondPath[strlen(path)];
  for (int k = 0; k < strlen(path); k++)
  {
    secondPath[k] = path[k];
  }

  // char *token; // in your original code allocate memory to pointer string
  // token = strtok(path, "/");
  // char *ch = "/";
  // size_t n;
  // n = 0;
  // while (token != NULL)
  // {

  //   if (strcmp(token, "USB1") == 0) // if "dog" found
  //     strcpy(&string[n], "/USB2");  // add "bird " at that position
  //   else if (strcmp(token, "USB2") == 0)
  //     strcpy(&string[n], "/USB1");
  //   // if doesn't add token
  //   else
  //   {

  //     printf("The token is %s\n", token);
  //     printf("the digit is %d\n", n);
  //     strcpy(&string[n], "/");
  //     strcpy(&string[n + 1], token);
  //   }
  //    n = strlen(string);
  //   token = strtok(NULL, "/");
  // }
  // printf("The token is %s\n", token);
  // printf("The string  is %s \n", &string);
  // processPutRequest(string, sockfd);
  // string[strlen(string)] = '\0';
  // int i = 0;
  // printf("\n$");

  // while (secondPath[i] != '\0')
  // {

  for (int i = 0; secondPath[i] != '\0'; i++)
  {
    // printf("(%c, %d) ", secondPath[i], i);
    // printf("test");
    if (secondPath[i] == '2')
    {
      secondPath[i] = '1';
      break;
    }
    else if (secondPath[i] == '1')
    {
      secondPath[i] = '2';
      break;
    }
    // else
    //   continue;
  }
  printf("test1");
  for (int k = 0; k < strlen(path); k++)
  {
    printf("(%c, %d) ", secondPath[k], k);
  }
  //  i++;
  // }
  printf("\n");

  return secondPath;
}

/*
 * Removes a file or folder from remote system.
 * @argument input arguments by user
 * @argument int socket
 * @returns void
 */
void processRmRequest(char *path, int sockfd)
{
  pthread_mutex_lock(&lock);
  printf("Entered in RM method\n");
  char *dirname = path;
  char *secondPath = processRequestForOtherDevice(path);
  char *server_message = malloc(200);
  int ret1, ret2 = 0;
  char cmd[200] = {0};
  ret1 = rmdir(dirname);
  ret2 = rmdir(secondPath);

  if (ret1 == 0)
  {
    strcpy(server_message, "Given empty directory removed successfully");
    printf("Given empty directory removed successfully\n");
  }
  else
  {
    //  strcpy (server_message,"Unable to remove directory");
    // printf("Unable to remove directory %s\n", dirname);
    sprintf(cmd, "rm -rf %s", dirname);

    if (system(cmd) == 0)
    {
      printf("Specified path folder/file has been deleted..\n");
      strcpy(server_message, "Non empty folder has been deleted..");
    }
    else
    {
      if (remove(path) == 0)
      {
        printf("Deleted file successfully\n");
        strcpy(server_message, "Deleted file successfully");
      }
      else
      {
        printf("Unable to delete anything.");
        strcpy(server_message, "Unable to delete anything.");
      }
    }

    if (send(sockfd, server_message, sizeof(server_message), 0) < 0)
    {
      printf("Can't send\n");
    }
  }

  // for other USB
  if (ret2 == 0)
  {
    strcpy(server_message, "Given empty directory removed successfully");
    printf("Given empty directory removed successfully\n");
  }
  else
  {
    //  strcpy (server_message,"Unable to remove directory");
    // printf("Unable to remove directory %s\n", dirname);
    sprintf(cmd, "rm -rf %s", secondPath);

    if (system(cmd) == 0)
    {
      printf("Specified path folder/file has been deleted..\n");
      strcpy(server_message, "Non empty folder has been deleted..");
    }
    else
    {
      if (remove(secondPath) == 0)
      {
        printf("Deleted file successfully\n");
        strcpy(server_message, "Deleted file successfully");
      }
      else
      {
        printf("Unable to delete anything.");
        strcpy(server_message, "Unable to delete anything.");
      }
    }

    if (send(sockfd, server_message, sizeof(server_message), 0) < 0)
    {
      printf("Can't send\n");
    }
  }
  pthread_mutex_unlock(&lock);
}

int rmrf(char *path)
{
  return nftw(path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
}

int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
  int rv = remove(fpath);

  if (rv)
    perror(fpath);

  return rv;
}