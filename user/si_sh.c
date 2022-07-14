// #include "functions.h"
#include "user.h"
#include "ucode.c"
#include "uio.c"
#include "string.c"
#include "crt0.c"

char *args[16];
// char *tokens[16];
char gpath[128];
int num_args;

// pipe vars:
int lpd[2];

void help()
{
    printf("COMMANDS IMPLEMENTED:");
    printf("--> sils, sigrep, sicat, |, >>, >, < \n");
}

int do_ioredirection(char *command);
int local_exec(char *command);
int do_pipe(char *cmd, int *pd);
int getLastPipeIndex(char *cmd);
int hasPipe(char **);

// here i need to handle the piping code.
int main(int argc, char *argv[])
{
    signal(SIGINT, IGNORE);
    int i = 0;
    int pid, status;
    char *char_ptr;
    // char *args;
    char usr_entry[64];
    char user_input[64];
    // printf("inside sh\n");s
    // // while(1);
    // STAT st;
    // stat("/", &st);
    // debug("root dir st.mode: %x\n", st.st_mode);

    // stat("/bin", &st);
    // debug("/bin dir st.mode: %x\n", st.st_mode);

    // stat("/f1", &st);
    // debug("/f1 file st.mode: %x\n", st.st_mode);

    // stat("/f", &st);
    // debug("/f file st.mode: %x\n", st.st_mode);
    // fstat(0, &st);
    // debug("stdin st.mode: %x\n", st.st_mode);

    // fstat(1, &st);
    // debug("stdout st.mode: %x\n", st.st_mode);
    help();
    while (1)
    {

        printf("silas shell >> ");
        gets(line);

        if (strcmp(line, "") == 0)
        {
            printf("No command was entered, try again\n");
            continue;
        }
        num_args = 0;

        // TODO: remove whitespace from usr_entry, make lowercase(?)

        printf("\n");
        strcpy(usr_entry, line);
        // strcpy(gpath, line);
        char_ptr = usr_entry;

        args[num_args++] = char_ptr;
        while (*char_ptr != 0)
        {
            kc_putc(*char_ptr);
            if (*char_ptr == ' ')
            {
                *char_ptr = 0;
                char_ptr++;
                args[num_args++] = char_ptr;
                // debug("arg[%d] = %s\n",num_args -1, args[num_args - 1]);
            }
            char_ptr++;
        }
        args[num_args] = 0;

        if (strcmp(args[0], "cd") == 0)
        {
            if (args[1] != 0)
                chdir(args[1]);
            else
                chdir("/");
            continue;
        }
        if (strcmp(args[0], "exit") == 0)
        {
            exit(1);
        }
        if (strcmp(args[0], "help") == 0)
        {
            help();
            continue;
        }
        pid = fork();
        if (pid)
        {
            pid = wait(&status);
            continue;
        }
        else
        {
            do_pipe(line, 0);
        }
    }
}

int getLastPipeIndex(char *cmd)
{
    char *cp = cmd;
    int pipe_index = 0;
    // char *pipe_offset = cmd;
    for (int i = 0; *cp != 0; cp++, i++)
    {
        if (*cp == '|')
        {
            pipe_index = i;
        }
    }
    return pipe_index;
}
/*
    pd[0] is for reading -> stdin
    pd[1] is for writing -> stdout

*/
int do_pipe(char *cmd, int *pd)
{
    // debug("inside do_pipe with: %s ", cmd);
    if (pd)
    {                   // writer
        close(pd[0]);   // close stdin
        dup2(pd[1], 1); // takes pd[1], and changes stdout to redirect there
        // close(pd[1]); // now that stdout is pointing at pd[1], we can close it
    }

    int lastPipeIndex = getLastPipeIndex(cmd);
    debug(" lastPipeIndex: %d\n", lastPipeIndex);
    if (lastPipeIndex == 0)
    {
        local_exec(cmd);
    }
    // cat f1 | grep printf
    //     -1 0 +1
    cmd[lastPipeIndex] = 0;
    cmd[lastPipeIndex - 1] = 0;
    cmd[lastPipeIndex + 1] = 0;
    char *head = cmd;                     // cat f1
    char *tail = cmd + lastPipeIndex + 2; // 2 because of space, tail = grep printf
    // debug("head: %s, tail: %s\n",head, tail);

    pipe(lpd);
    int pid = fork();
    // assume parent is writer pd[1], child is reader pd[0]
    if (pid)
    { // parent
        close(lpd[1]);
        dup2(lpd[0], 0);
        // close(lpd[0]);
        // debug("in parent executing tail: %s\n", tail);
        local_exec(tail);
    }
    else
    { // child
        // debug("in child calling do_pipe with: %s\n", head);
        do_pipe(head, lpd);
    }
}

int local_exec(char *cmdarr)
{
    // debug("local_exec with: %s\n", cmdarr);
    int io = do_ioredirection(cmdarr);
    exec(cmdarr);
}

int do_ioredirection(char *cmd)
{
    int i;
    char *cp = cmd;

    while (*cp != 0)
    {
        if (*cp == '>' && *(cp + 1) == '>')
        {
            debug("found >> ");
            *(cp - 1) = 0;
            *(cp) = 0;
            *(cp + 1) = 0;
            *(cp + 2) = 0;
            cp += 3; // if *cp == > then cp + 3 is file name
            debug("closing stdout, redirecting to %s\n", cp);
            close(1); // close stdout
            open(cp, O_WRONLY | O_APPEND | O_CREAT);
            return 1;
        }
        else if (*cp == '>')
        {
            debug("found > ");
            *(cp - 1) = 0;
            *(cp) = 0;
            *(cp + 1) = 0;
            cp += 2; // if *cp == > then cp + 2 is file name
            debug("closing stdout, redirecting to %s\n", cp);
            close(1); // close stdout
            open(cp, O_CREAT | O_WRONLY | O_TRUNC);
            return 1;
        }
        else if (*cp == '<')
        {
            debug("found < ");
            *(cp - 1) = 0;
            *(cp) = 0;
            *(cp + 1) = 0;
            cp += 2; // if *cp == > then cp + 2 is file name
            debug("closing stdin, getting input from %s\n", cp);
            close(0); //// close stdin
            open(cp, O_RDONLY);
            return 1;
        }
        cp++;
    }
    debug("returnign 0 from do_ioredirection\n");
    return 0;
}