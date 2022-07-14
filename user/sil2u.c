#include "user.h"
#include "ucode.c"
#include "uio.c"
#include "string.c"
#include "crt0.c"




char input[MAX_LINE_LENGTH];
int convert() {
    char *cp = input;
    while (*cp){
        if ((int)*cp > 96 && (int) *cp < 122) {
            *cp = *cp - 32; 
        }
        cp++;
    }
}

bool inputNeedsEcho = false;
bool outputNeedsNewline = false;
char * src = 0;
char * dst = 0;

int main(int argc, char *argv[])
{

    int i = 0;
    if (argc == 2) exit(1);

    STAT ist;
    fstat(0, &ist);
    
    STAT ost;
    fstat(1, &ost);
    
    inputNeedsEcho = ist.st_mode == ST_TERMINAL;
    outputNeedsNewline = ost.st_mode == ST_TERMINAL;
    
    if (argc == 3) {
        src = argv[1];
        dst = argv[2];
        close(0);
        open(src, O_RDONLY); // opening stdin
        close(1);
        open(dst, O_WRONLY); // opening stdout
    }
    
    int num_chars = (inputNeedsEcho) ? gets(input) :getline(input) ;
    while(num_chars > 0) {
        convert();
        if(outputNeedsNewline) {
            printf("%s\n", input);
        }else{
            puts_nonewline(input);
        }

        num_chars = (inputNeedsEcho) ? gets(input) : getline(input); //  : gets(input);
    }




    exit(0);

}
