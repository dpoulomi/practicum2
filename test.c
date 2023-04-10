// /*
//  * This program displays the names of all files in the current directory.
//  */

// #include <dirent.h>
// #include <stdio.h>

// #include <stdio.h>
// #include <string.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <unistd.h>
// #include <dirent.h>
// #include <string.h>
// #include <sys/stat.h>
// #include <stdlib.h>

// int main(void)
// {
//     DIR *d;
//     struct dirent *dir;
//     d = opendir("/Users/neo/CS5600/practicum_server/");
//     if (d)
//     {
//         while ((dir = readdir(d)) != NULL)
//         {
//             if (dir->d_type == DT_REG && strcmp("file", dir->d_name) == 0)
//             {
//                 printf("%s\n", dir->d_name);
//             }
//         }
//         closedir(d);
//     }
//     return (0);
// }

#include <stdio.h>
#include <string.h>

char *remove_end(char *str, char c)
{
   char *last_pos = strrchr(str, c);

   if (last_pos != NULL) {
      *last_pos = '\0';
      return last_pos + 1; /* pointer to the removed part of the string */
   }

   return NULL;  /* c wasn't found in the string */
}

int main(void)
{
   char s1[] = "/Users/neo/CS5600/practicum_server/file";
//    char s2[] = "nothing to remove";
   char *removed;

   /* s1 */
   printf("Original string: %s\n", s1);
   removed = remove_end(s1, '/');
   printf("New string: %s\n", s1);
   printf("Removed: %s\n", removed ? removed : "NOTHING");
   /* s2 */
//    printf("Original string: %s\n", s2);
//    removed = remove_end(s2, ';');
//    printf("New string: %s\n", s2);
//    printf("Removed: %s\n", removed ? removed : "NOTHING");

   return 0;
}
