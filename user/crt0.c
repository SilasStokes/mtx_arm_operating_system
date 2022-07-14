/* crt0.c : main0(s) called from u.s, where s = oigianl command string
            tokenlize s into char *argv[ ] and call main(argc, argv).
 
    token() breaks up a string into argc of tokens, pointed by argv[]
*/
/*  #include "uinclude.h" */

// int argc;
// char *argv[32];

#include "user.h"
int argc;
char *argv[32];

void token(char *line)
{
    char *cp;
    cp = line;
    argc = 0;

    while (*cp != 0)
    {
        while (*cp == ' ')
            *cp++ = 0;
        if (*cp != 0)
            argv[argc++] = cp;
        while (*cp != ' ' && *cp != 0)
            cp++;
        if (*cp != 0)
            *cp = 0;
        else
            break;
        cp++;
    }
    argv[argc] = 0;
}

void showarg(int argc, char *argv[])
{
    int i;
    printf("argc=%d ", argc);
    for (i = 0; i < argc; i++)
        //printf("argv[%d]=%s ", i, argv[i]);
        printf("%s ", argv[i]);
    prints("\n");
}
// BEFORE: r0 was trashed in goUmode(), so had to rely on r1->string
// NOW: r0 is NOT trashed in goUmode() ==> should be r0 alone
void main0(char *s)
{
    if (s)
    {
        //printf("s=%s\n", s);
        token(s);
    }
    //showarg(argc, argv);
    main(argc, argv);
}
