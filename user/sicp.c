#include "user.h"
#include "ucode.c"
#include "uio.c"
#include "string.c"
#include "crt0.c"


int main(int argc, char *argv[])
{

    int i;
    for (i = 0; i < argc; i++)
    {
        debug("argv[%d] = %s\n", i, argv[i]);
    }
    if (argc < 3) {
        printf("please use format: cp source destination. Exiting. . .\n");
    }
    char *src  = argv[1];
    char *dest = argv[2];

    STAT st;
    stat(src, &st);
    if ( (st.st_mode  & 0xF000) != 0x8000) { // if it's not a regular file
        printf("ERR.. not a L2U-able file\n");
        exit(1);
    }
    close(0); // closing stdin so I can read from it
    close(1);
    int sfd = open(src, O_RDONLY);
    int dfd = open(dest, O_CREAT | O_WRONLY | O_TRUNC);
    // printf("sfd: %d, dfd:%d \n", sfd ,dfd);

    char buf[MAX_LINE_LENGTH];
    
    int num_bytes = getline(buf);
    while (num_bytes )
    {
        puts_nonewline(buf);
        num_bytes = getline(buf);
    }
}
