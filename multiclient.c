#include <stdio.h>
#include "fget.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>


struct multiexecution_clientparams
{
	char** userInput;
    int argc;
};

int main()
{

	pthread_t tid[22];

	struct multiexecution_clientparams param;
	param.userInput = "fget PUT /Users/neo/CS5600/practicum_client/getUSBDATA.txt new_folder_from_start/data10.txt";
	param.argc = 4;
	pthread_create(&(tid[0]), NULL, &handleConnection, (void *)&param);
	pthread_join(tid[0], NULL);
	// printf("the pageId is : %ld \n", address);

	// // struct multiexecution_clientparams param;
	// param.userInput = "fget PUT /Users/neo/CS5600/practicum_client/getUSBDATA.txt new_folder_from_start/data11.txt";
	// param.argc = 4;
	// pthread_create(&(tid[1]), NULL, &handleConnection, (void *)&param);
	// pthread_join(tid[1], NULL);

	// // struct multiexecution_clientparams param;
	// param.userInput = "fget GET /new_folder_from_start/data11.txt /Users/neo/CS5600/practicum_client/test_get_1.txt";
	// param.argc = 4;
	// pthread_create(&(tid[2]), NULL, &handleConnection, (void *)&param);
	// pthread_join(tid[2], NULL);

	// // struct multiexecution_clientparams param;
	// param.userInput = "fget MD test_multithreading";
	// param.argc = 3;
	// pthread_create(&(tid[3]), NULL, &handleConnection, (void *)&param);
	// pthread_join(tid[3], NULL);

	// // struct multiexecution_clientparams param;
	// param.userInput = "fget RM test_multithreading";
	// param.argc = 3;
	// pthread_create(&(tid[4]), NULL, &handleConnection, (void *)&param);
	// pthread_join(tid[4], NULL);

	// param.userInput = "fget PUT /Users/neo/CS5600/practicum_client/getUSBDATA.txt new_folder_from_start/data14.txt";
	// param.argc = 4;
	// pthread_create(&(tid[5]), NULL, &handleConnection, (void *)&param);
	// pthread_join(tid[5], NULL);

	// // struct multiexecution_clientparams param;
	// param.userInput = "fget INFO new_folder_from_start/data14.txt";
	// param.argc = 3;
	// pthread_create(&(tid[6]), NULL, &handleConnection, (void *)&param);
	// pthread_join(tid[6], NULL);


	// pthread_exit(NULL);

	return 0;
}