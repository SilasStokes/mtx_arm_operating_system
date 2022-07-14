#include "ucode.c"

int main(int argc, char *argv[])
{
    //   need to add `int fe (char *line){return syscall(70,line,0);}`

    printf("INSIDE fe.c\n\n");
    int i = 0;
    if (argc <2) {
        printf("Not enough arguments");
        exit(1);
    }
    char * command = argv[1];
    for (i = 2; i < argc; i ++) {
        char * index = argv[i];
        index--;
        *index = ' ';
    }
    printf("command = %s\n",command);

    // int i;

    // for (i = 0; i < argc; i++)
    // {
    //     printf("argv[%d] = %s\n", i, argv[i]);
    // }
    // printf("line = %s\n");
    int status = 0;
    fe(command);
    pid = wait(&status);
    printf("Zombie child %d, status %x\n", pid, status);
    exit(0);
}

// int fe(char * command_line) {

// }

// all user interface is in ucode.c
//      need to add `int fe (char *line){return syscall(70,line,0);}`
//      this will call the vector table in svc.c
//
//      he put kfe in fork.c file
//      int i, j;
//      char *cp, *cq;
//         u32 *ucp,
// readhow fork works

// int main (int argc, char *argv[]) {
//     pid = fe(line);
//     pid = wait (&status);
//     printf("Zombie child:%d, status %x,\n")
//     exit(0);
// }