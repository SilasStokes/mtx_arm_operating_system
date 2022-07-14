#include "user.h"
#include "ucode.c"
#include "uio.c"
#include "string.c"
#include "crt0.c"

char input[1024];
bool inputNeedsEcho = false;
bool outputNeedsNewline = false;
char * src = 0;
char * dst = 0;

int main(int argc, char *argv[])
{
    int i = 0;
    char line[64];


    STAT ist;
    fstat(0, &ist);
    
    STAT ost;
    fstat(1, &ost);
    // debug("ist.mode = %x\n", ist.st_mode);
    // debug("ost.mode = %x\n", ost.st_mode);
    
    inputNeedsEcho = ist.st_mode == ST_TERMINAL && argv[1] == 0 ;
    // debug("input needs echo: %d\n", inputNeedsEcho);
    outputNeedsNewline = ost.st_mode == ST_TERMINAL;
    
    if (argc == 2) {
        src = argv[1];
        STAT st;
        stat(src, &st);
        // debug("st.mode of file %s = %x\n", paTh, st.st_mode);
        if ( (st.st_mode  & 0xF000) != 0x8000) { // if it's not a regular file
            printf("ERR.. not a cat-able file\n");
            exit(1);
        }
        close(0);
        open(src, O_RDONLY); // opening stdin
    }

    int num_chars = (inputNeedsEcho) ? gets(input) : getline(input);
    while(num_chars > 0) {
        if (outputNeedsNewline){
            printf("%s", input);
        } else {
            puts_nonewline(input);
        }

        num_chars = (inputNeedsEcho) ? gets(input) : getline(input);
    }


    // int num_bytes = read(fd, buf, 1024);
    // debug("fd = %d, number of bytes read: %d\n", fd, num_bytes);
    // char *cp = buf;
    // while (num_bytes > 0 ) {
    //     while (cp < buf + num_bytes) {
    //         if (fdst.st_mode == 0x21A4) {
    //             kc_putc(*cp);
    //         } else{
    //             putc_nonewline(*cp);
    //         }
    //         cp++;
    //     }
    //     num_bytes = read(fd, buf, 1024);
    //     cp = buf;
    // }

    // debug("Exiting sicat...\n");
    exit(0);
}
