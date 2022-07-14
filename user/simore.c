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

int inputfiledesc = 0;
int fd;

void clear() {
    for (int i = 0; i < 24; i++) {
        printf("\n");
    }

}

int outputpage() {
    // int num_chars = (inputNeedsEcho) ? gets(input) : getline(input);
    int num_chars = getline_file(input, inputfiledesc);
    for (int i = 0; i < 24 ; i++) {
        if (num_chars == 0) return 0;
        if (outputNeedsNewline){
            printf("%s", input);
        } else {
            puts_nonewline(input);
        }
        // num_chars = (inputNeedsEcho) ? gets(input) : getline(input);
        num_chars = getline_file(input, inputfiledesc);
    }
    return 1;

}

int outputline() {
    // int num_chars = (inputNeedsEcho) ? gets(input) : getline(input);
    int num_chars = getline_file(input, inputfiledesc);
    if (num_chars == 0) return 0;
    if (outputNeedsNewline){
        printf("%s", input);
    } else {
        puts_nonewline(input);
    }
    return num_chars;
}



int main(int argc, char *argv[]) {
    STAT ist;
    fstat(0, &ist);
    
    STAT ost;
    fstat(1, &ost);
    
    inputNeedsEcho = ist.st_mode == ST_TERMINAL;
    outputNeedsNewline = ost.st_mode == ST_TERMINAL;

    if (ist.st_mode == PIPE_MODE) {
        inputfiledesc = dup(0);
        close(0);
        int newstdin = open("/dev/tty0", O_RDONLY);
        debug("there was a pipe so redirecting stdin to %d, open(devtty0) = %d\n", inputfiledesc, newstdin);
    } else if (argc ==2){
        src = argv[1];
        inputfiledesc = open(src, O_RDONLY);
    } else {
        exit(1);
    }
    
    int resp = outputpage();
    do {
        int c = getc();
        if ((char)c == ' ') {
            resp = outputline();
        } else if (c == 13) {
            resp = outputpage();
        } else {
            printf("char entered: %d\n", c);
        }
    } while(resp);

    exit(0);
}
