#include "helpers.h"

#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <stdlib.h>
#include <stdio.h>




#define MK_PATH_CMD "mkdir -p "


int make_directory(char *path){
    int path_len = strlen(path);
    int MK_PATH_CMD_LEN = strlen(MK_PATH_CMD);

    char *command = malloc( sizeof(char)*(MK_PATH_CMD_LEN + path_len + 1) ); // plus 2 is for seperating '/' and null terminator
    
    sprintf(command,"%s%s",MK_PATH_CMD,path);

    puts(command);
    
    if(system(command) == -1){
        free(command);
        perror("Error");
        return 0;
    }

    free(command);
    return 1;
}
