The fget.c is the client program and server.c is the server program.

1)The code to allow you to send a command to the server for reading a file from the server's file storage area. The request should include some kind of command (e.g., GET) and a path to a file name (e.g., folder/foo.txt). 
Eg:-./fget GET /Volumes/USB1/new_folder_from_start/data7.txt /Users/neo/CS5600/practicum_client/getUSBDATA.txt

2)INFO command that gets all file information about a file, e.g., INFO folder/path/file.txt. It would return relevant information such as ownership, data of last modification, permissions, size, etc. The program then prints all of the file information returned.
Eg:-./fget INFO /Volumes/USB1/new_folder_from_start/data7.txt

3) MD command that creates a folder in the server. It returns success int value as an indication if the make directory action is successful or not -- Eg:-./fget MD /Volumes/USB1/new_folder

4)PUT command creates a new file in the remote file system, and stores a stream of bytes in the file. If the remote file path or name is omitted,the local path or file is used--Eg:-./fget PUT /Users/neo/CS5600/practicum_client/getUSBDATA.txt  /Volumes/USB1/new_folder_from_start/data6.txt

5)RM command deletes a file or empty folder or non empty folder in the remote file system and returns success int value as an indication if the remove directory action is successful or not--Eg:- ./fget RM /Volumes/USB1/new_folder  or ./fget RM /Volumes/USB1/new_folder/data.txt

6)Every operation happens in a way that we have a completely synced up mirrored file server system. If we put a file in one USB ot gets written to the other as well. If GET does not fetch from a specifc server(USB), it will fetch from the other. When a server is back, it gets synced up again forming a mirrored file system.

7)Multiple client requests are handled by forking i.e by creating multiple processes.

