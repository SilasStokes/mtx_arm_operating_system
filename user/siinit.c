
#include "user.h"
#include "crt0.c"
#include "string.c"
#include "uio.c"
#include "ucode.c"


char *serialstr[] = {"/dev/ttyS0", "dev/ttyS1", 0};
char *login = "si_login";

int console;
int serial0, serial1;
void parent()
{
    int pid, status;
    while(1) {
        printf("INIT : wait for ZOMBIE child\n");
        pid = wait(&status);
        if (pid == console) {
            printf("init forks a new console login");
            console = fork();
            if(console) 
                continue;
            else
                exec("si_login /dev/tty0");
        }
        printf("burried orphan process %d\n", pid);
    }
}


void openserial(int index){
    char temp[64];
    int serial = fork();
    if (serialstr[index] ==0 ){
        parent();
    }
    if (serial) {
        openserial(index + 1);
    } else {
        strcpy(temp, login);
        strcat(temp, " ");
        strcat(temp, serialstr[index]);
        exec(temp);
    }
}

int main(int argc, char *argv[])
{
    // printf("SI_INIT \n");
    int stdinput, stdoutput, stderror;
    int pid;
    stdinput = open("/dev/tty0", O_RDONLY);
    stdoutput = open("/dev/tty0", O_WRONLY);
    stderror = open("/dev/tty0", O_WRONLY);
    console = fork();
    if (console) { 
        // openserial(0);
        parent();
    } else {   
        exec("si_login /dev/tty0");
    }
}
