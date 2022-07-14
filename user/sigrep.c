#include "user.h"
#include "ucode.c"
#include "uio.c"
#include "string.c"
#include "crt0.c"

char * file_name;
char * str;
//          0       1       2
// case 1: grep     abcd
// case 2: grep     printf  f
char grepline[128];
char *cp;
int nMatchChars;
int matchIndex;

int getNumCharsToMatch(char *lstr){
    char *lcp = lstr;
    while (*lcp) {
        lcp++;
    }
    // debug("lstr = %x, lcp = %x, lcp-lstr = %d", lstr, lcp, lcp-lstr);
    return lcp - lstr ;
}

int main(int argc, char *argv[])
{

    int i = 0;
    matchIndex = 0;
    // debug("INSIDE SIGREP.\n");
    for (i = 0; i < argc; i++)
    {
        debug("argv[%d] = %s\n", i, argv[i]);
    }

    if (argc < 2) {
        printf("Pass string to parse for\n");
        exit(1);
    }
    STAT st;
    fstat(1, &st);
    int outputMode = st.st_mode;

    str = argv[1];
    // cp = argv[1];
    nMatchChars = getNumCharsToMatch(str);
    // debug("num chars to match: %d\n", nMatchChars);

    // if a file is passed, redirect stdin
    int ifstdin = 0;
    if (argc == 3){
        ifstdin = 1;
        close(0);
        file_name = argv[2];
        open(file_name, O_RDONLY);
    }
    STAT ist; // input stat
    fstat(0, &ist);
    int ifinputpipe = 0; 
    if (ist.st_mode == ST_TERMINAL_IN) {
        ifinputpipe = 1;
    }

    i = 0;
    //if the input is a file or it's from a redirected stdin, don't echo by using getline
    int num_chars = (ifstdin || ifinputpipe) ? getline(grepline) : gets(grepline); // : getline(grepline);
    while ( num_chars > 0) {
        // debug("number of chars: %d, line %s\n", grepline, num_chars);
        cp = grepline;
        while (cp < grepline + num_chars) {
            if (*cp == str[matchIndex++] ) {
                if (matchIndex == nMatchChars) {
                    if (outputMode == ST_TERMINAL) {
                        printf("%s\n", grepline);
                    } else {
                        printf("%s", grepline);
                        // print2f(grepline);
                    }
                    matchIndex = 0;
                }
            } else{
                matchIndex = 0;
            }
            cp++;
        }
        matchIndex = 0;
        printf("\n");
        num_chars = (ifstdin || ifinputpipe) ? getline(grepline) : gets(grepline); // : getline(grepline);
    }

    exit(0);

}
