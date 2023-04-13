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
#define SIZE 1024

void processClientRequest(int client_sock, char client_message[8196], int socket_desc);
void processGetRequest(char *dir, int sockfd);
void processInfoRequest(char *path, int sockfd);
void send_file(FILE *fp, int sockfd);
void processMDRequest(char *path, int sockfd);
void processPutRequest(char *path, int sockfd);
char *processPutRequestForOtherDevice(char *path, int sockfd);
void processRmRequest(char *path, int sockfd);
int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);
int rmrf(char *path);
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
  processClientRequest(client_sock, client_message, socket_desc);
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
  printf("array 0: %s\n", array[0]);
  printf("array 1: %s\n", array[1]);
  if (strcmp(array[0], "GET") == 0)
  {
    processGetRequest(array[1], client_sock);
  }
  else if (strcmp(array[0], "INFO") == 0)
  {
    printf("Entering info block");
    processInfoRequest(array[1], client_sock);
  }
  else if (strcmp(array[0], "MD") == 0)
  {
    processMDRequest(array[1], client_sock);
  }
  else if (strcmp(array[0], "PUT") == 0)
  {
    processPutRequest(array[1], client_sock);
  }
  else if (strcmp(array[0], "RM") == 0)
  {
    processRmRequest(array[1], client_sock);
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

void processGetRequest(char *path, int sockfd)
{
  FILE *fp;
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

void processInfoRequest(char *path, int sockfd)
{
  printf("ENtered in info method");
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
    printf("Unable to get file properties.\n");
    printf("Please check whether '%s' file exists.\n", path);
  }
}

void processMDRequest(char *path, int sockfd)
{
  printf("Entered in MD method\n");
  int check;
  char *dirname = path;
  char server_message[2000];
  memset(server_message, '\0', sizeof(server_message));
  check = mkdir(dirname, 0777);
  // check if directory is created or not
  if (!check)
  {
    strcpy(server_message, "Directory created.");
    printf("Directory created\n");
  }
  else
  {
    strcpy(server_message, "Unable to create directory.");
    printf("Unable to create directory\n");
    exit(1);
  }

  if (send(sockfd, server_message, sizeof(server_message), 0) < 0)
  {
    printf("Can't send\n");
  }
}

void processPutRequest(char *path, int sockfd)
{
  FILE *fp;
  char *filename = path;
  //  printf("Original filepath is: %s", path);
  char buffer[SIZE];
  int src_fd, dst_fd, err;
  src_fd = open(path, O_RDONLY);

  fp = fopen(filename, "w");
  printf("filename :%s $\n", filename);
  char *secondPath = processPutRequestForOtherDevice(path, sockfd);
  printf("second filename is %s $ and length: %d\n", secondPath, strlen(secondPath));

  int n;

  FILE *fp1;
  // char *filename = path;

  fp1 = fopen(secondPath, "w"); // "/Volumes/USB2/test_folder4/test_server_put152.c"
  while (1)
  {
    n = recv(sockfd, buffer, SIZE, 0);
    if (n <= 0)
    {
      break;
    }
    // printf("First Buffer: %s\n", buffer);
    fprintf(fp, "%s", buffer);
    // printf("Second Buffer: %s\n", buffer);
    fprintf(fp1, "%s", buffer);
    //  printf( "Buffer is %s", buffer);
  
    bzero(buffer, SIZE);
  }
  fclose(fp);
  fclose(fp1);

 
  // int n1=0;
  //   dst_fd = open(secondPath, O_CREAT | O_WRONLY);

  //   while (1) {
  //       err = read(src_fd, buffer, 4096);
  //       if (err == -1) {
  //           printf("Error reading file.\n");
  //           exit(1);
  //       }
  //       n1 = err;

  //       if (n1 == 0) break;

  //       err = write(dst_fd, buffer, n1);
  //       if (err == -1) {
  //           printf("Error writing to file.\n");
  //           exit(1);
  //       }
  //   }

  //   close(src_fd);
  //   close(dst_fd);

  return;
}

char *processPutRequestForOtherDevice(char *path, int sockfd)
{
  char string[99];

  char *token; // in your original code allocate memory to pointer string
  token = strtok(path, "/");
  char *ch = "/";
  size_t n;
  n = 0; 
  while (token != NULL)
  {
    // char temp[20];
               // calculate string length
    if (strcmp(token, "USB1") == 0) // if "dog" found
      strcpy(&string[n ], "/USB2");  // add "bird " at that position
    else if (strcmp(token, "USB2") == 0)
      strcpy(&string[n], "/USB1");
    // if doesn't add token
    else
    {

      printf("The token is %s\n", token);
      printf("the digit is %d\n", n);
      strcpy(&string[n], "/");
      strcpy(&string[n + 1], token);
    }
     n = strlen(string); 
    token = strtok(NULL, "/");
  }
  printf("The token is %s\n", token);
  printf("The string  is %s \n", &string);
  // processPutRequest(string, sockfd);
  //string[strlen(string)] = '\0';
  int i = 0;
  // printf("\n$");

  while (string[i] != '\0'){
    printf("(%c, %d) ", string[i], i);
    i++;
  }
  printf("\n");

  return string;

}



void processRmRequest(char *path, int sockfd)
{
  printf("Entered in RM method\n");
  char *dirname = path;
  char *server_message = malloc(200);
  int ret = 0;
  char cmd[200] = {0};
  ret = rmdir(dirname);

  if (ret == 0)
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
