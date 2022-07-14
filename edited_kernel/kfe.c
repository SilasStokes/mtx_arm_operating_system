

#include "../type.h"
// extern int exec(char *);
extern int goUmode();
// 
int kfe(char *uline){
    // fork a child proc with identical Umode image=> same u1 file
    int i, j;
    char *cp, *cq;
    u32 *ucp, *upp;
    u32 PBA, CBA;

    PROC *p = getproc();
    if (p == 0)
    {
        kprintf("fork failed\n");
        return -1;
    }
    //printf("new=%d pgdir=%x\n", p->pid, p->res->pgdir);

    /* initialize the new PROC and its kstack */
    p->status = READY;
    p->ppid = running->pid;
    p->parent = running;
    p->priority = 1;
    p->inkmode = 1;
    p->time = 0;
    p->cpu = 0;
    p->type = PROCESS;
    p->cpsr = 0x10;

    p->res->size = running->res->size;
    p->res->uid = running->res->uid;
    p->res->gid = running->res->gid;
    p->res->cwd = running->res->cwd;
    p->tcount = 1;
    p->res->cwd->refCount++;
    strcpy(p->res->tty, running->res->tty);

    // p->res->signal, p->res->sig[] are cleared in kexit()
    p->res->signal = 0;
    for (i = 0; i < NSIG; i++)
        p->res->sig[i] = 0;
    /***** clear message queue ******/
    p->res->mqueue = 0;
    p->res->mlock.value = 1;
    p->res->mlock.queue = 0;
    p->res->message.value = 0;
    p->res->message.queue = 0;

    //printf("running usp=%x linkR=%x\n", running->usp, running->upc);

    PBA = (running->res->pgdir[2048] & 0xFFF00000);
    //printf("FORK: parent %d uimage at %x\n", running->pid, PBA);

    CBA = (p->res->pgdir[2048] & 0xFFF00000);
    // printf("FORK: child  %d uimage at %x\n", p->pid, CBA);

    //printf("FORK: copy Umode image from %x to %x\n", PBA, CBA);
    // copy 1MB of Umode image
    upp = (int *)PBA;
    ucp = (int *)CBA;
    for (i = 0; i < 0x100000 / 4; i++)
    {
        *ucp++ = *upp++;
    }
    p->upc = running->upc;
    p->usp = running->usp; // both should be VA in their sections

    // the hard part: child must resume to the same place as parent
    // child kstack must contain |parent kstack|goUmode stack|=> copy kstack
    //printf("copy kernel mode stack\n");
    // j = &running->kstack[SSIZE] - running->ksp;

    // kstack syscall frame must be copied from parent's kstack, set R0=0
    //  1   2   3   4   5  6  7  8  9  10 11 12 13 14
    // ------------------------------------------------
    //  PC r12 r11 r10  r9 r8 r7 r6 r5 r4 r3 r2 r1 R0  |
    //-------------------------------------------------

    for (i = 1; i <= 14; i++)
    {
        p->kstack[SSIZE - i] = running->kstack[SSIZE - i];
        //printf("%x ", p->kstack[SSIZE-i]);
    }

    p->kstack[SSIZE - 14] = 0; // child return 0 pid in saved r0

    //printf("FIX UP child resume PC to %x\n", running->upc);
    p->kstack[SSIZE - 1] = (int)(running->upc);
    p->kstack[SSIZE - 15] = (int)goUmode;
    p->ksp = &(p->kstack[SSIZE - 28]);

    // printf("copy file descriptors\n");
    /**** copy file descriptors ****/
    for (i = 0; i < NFD; i++)
    {
        p->res->fd[i] = running->res->fd[i];
        if (p->res->fd[i] != 0)
        {
            //printf("copy file descriptor %d\n", i);
            p->res->fd[i]->refCount++;
            if (p->res->fd[i]->mode == READ_PIPE)
            {
                p->res->fd[i]->pipe_ptr->nreader++;
                //printf("nreader=%d ", p->res->fd[i]->pipe_ptr->nreader);
            }
            if (p->res->fd[i]->mode == WRITE_PIPE)
            {
                p->res->fd[i]->pipe_ptr->nwriter++;
                //printf("nwriter=%d ", p->res->fd[i]->pipe_ptr->nwriter);
            }
        }
    }


    // enqueue(&readyQueue, p);

    // kprintf("FORK: proc %d forked a child %d\n", running->pid, p->pid);
    // printQ(readyQueue);

    // return p->pid;
// DOING EXEC:

    char kline[64];
    char file[32], filename[64];
    int *usp, *ustacktop;
    u32 BA, Btop, Busp, uLINE;
    char line[32];

    //printf("EXEC: proc %d uline=%x ", running->pid, uline);

    // line is in Umode image at p->pgdir[2048]&0xFFF00000=>can access from Kmode
    // char *uimage = (char *)(p->pgdir[2048] & 0xFFF00000);
    BA = (p->res->pgdir[2048] & 0xFFFFF000);
    Btop = BA + 0x100000; // top of 1MB Uimage
    //printf("EXEC: proc %d Uimage at %x\n", running->pid, BA);

    uLINE = BA + ((int)uline - 0x80000000);
    kstrcpy(kline, (char *)uLINE);
    // NOTE: may use uline directly

    //printf("EXEC: proc %d line = %s   ", running->pid, kline);

    // first token of kline = filename
    cp = kline;
    i = 0;
    while (*cp != ' ' && *cp)
    {
        filename[i] = *cp;
        i++;
        cp++;
    }
    filename[i] = 0;
    /*
  kstrcpy(file, "/bin/");
  kstrcat(file, filename);
  */
    BA = p->res->pgdir[2048] & 0xFFFFF000;
    //kprintf("load file %s to %x\n", file, BA);

    // load filename to Umode image
    if (load(filename, p) <= 0)
    {
        printf("exec loading error\n");
        return -1;
    }
    //  printf("after loading ");

    // copy kline to high end of Ustack in Umode image
    Btop = BA + 0x100000;
    Busp = Btop - 32;

    cp = (char *)Busp;
    kstrcpy(cp, kline);
    //printf("cp=%x contents=%s\n", cp, cp);

    p->usp = (int *)VA(0x100000 - 32);
    p->upc = (int *)VA();

    p->kstack[SSIZE - 14] = (int)VA(0x100000 - 32); // R0 to Umode
    p->kstack[SSIZE - 1] = (int)VA(0);              // ulr to Umode
    //printf("usp=%x contents=%s\n", p->usp, (char *)p->usp);
    strcpy(running->res->name, filename);

    //kprintf("kexec exit\n");
    return p->pid; // return value may ovewrite saved R0 in kstack

}
// int kfe(char *uline)
// {
//     // fork a child proc with identical Umode image=> same u1 file
//     int i, j;
//     char *cp, *cq;
//     u32 *ucp, *upp;
//     u32 PBA, CBA;

//     PROC *p = getproc();
//     if (p == 0)
//     {
//         kprintf("fork failed\n");
//         return -1;
//     }
//     //printf("new=%d pgdir=%x\n", p->pid, p->res->pgdir);

//     /* initialize the new PROC and its kstack */
//     p->status = READY;
//     p->ppid = running->pid;
//     p->parent = running;
//     p->priority = 1;
//     p->inkmode = 1;
//     p->time = 0;
//     p->cpu = 0;
//     p->type = PROCESS;
//     p->cpsr = 0x10;

//     p->res->size = running->res->size;
//     p->res->uid = running->res->uid;
//     p->res->gid = running->res->gid;
//     p->res->cwd = running->res->cwd;
//     p->tcount = 1;
//     p->res->cwd->refCount++;
//     strcpy(p->res->tty, running->res->tty);

//     // p->res->signal, p->res->sig[] are cleared in kexit()
//     p->res->signal = 0;
//     for (i = 0; i < NSIG; i++)
//         p->res->sig[i] = 0;
//     /***** clear message queue ******/
//     p->res->mqueue = 0;
//     p->res->mlock.value = 1;
//     p->res->mlock.queue = 0;
//     p->res->message.value = 0;
//     p->res->message.queue = 0;

//     //printf("running usp=%x linkR=%x\n", running->usp, running->upc);

//     PBA = (running->res->pgdir[2048] & 0xFFF00000);
//     //printf("FORK: parent %d uimage at %x\n", running->pid, PBA);

//     CBA = (p->res->pgdir[2048] & 0xFFF00000);
//     // printf("FORK: child  %d uimage at %x\n", p->pid, CBA);

//     //printf("FORK: copy Umode image from %x to %x\n", PBA, CBA);
//     // copy 1MB of Umode image
//     upp = (int *)PBA;
//     ucp = (int *)CBA;
//     for (i = 0; i < 0x100000 / 4; i++)
//     {
//         *ucp++ = *upp++;
//     }
//     p->upc = running->upc;
//     p->usp = running->usp; // both should be VA in their sections

//     // the hard part: child must resume to the same place as parent
//     // child kstack must contain |parent kstack|goUmode stack|=> copy kstack
//     //printf("copy kernel mode stack\n");
//     // j = &running->kstack[SSIZE] - running->ksp;

//     // kstack syscall frame must be copied from parent's kstack, set R0=0
//     //  1   2   3   4   5  6  7  8  9  10 11 12 13 14
//     // ------------------------------------------------
//     //  PC r12 r11 r10  r9 r8 r7 r6 r5 r4 r3 r2 r1 R0  |
//     //-------------------------------------------------

//     for (i = 1; i <= 14; i++)
//     {
//         p->kstack[SSIZE - i] = running->kstack[SSIZE - i];
//         //printf("%x ", p->kstack[SSIZE-i]);
//     }

//     p->kstack[SSIZE - 14] = 0; // child return 0 pid in saved r0

//     //printf("FIX UP child resume PC to %x\n", running->upc);
//     p->kstack[SSIZE - 1] = (int)(running->upc);
//     p->kstack[SSIZE - 15] = (int)goUmode;
//     p->ksp = &(p->kstack[SSIZE - 28]);

//     // printf("copy file descriptors\n");
//     /**** copy file descriptors ****/
//     for (i = 0; i < NFD; i++)
//     {
//         p->res->fd[i] = running->res->fd[i];
//         if (p->res->fd[i] != 0)
//         {
//             //printf("copy file descriptor %d\n", i);
//             p->res->fd[i]->refCount++;
//             if (p->res->fd[i]->mode == READ_PIPE)
//             {
//                 p->res->fd[i]->pipe_ptr->nreader++;
//                 //printf("nreader=%d ", p->res->fd[i]->pipe_ptr->nreader);
//             }
//             if (p->res->fd[i]->mode == WRITE_PIPE)
//             {
//                 p->res->fd[i]->pipe_ptr->nwriter++;
//                 //printf("nwriter=%d ", p->res->fd[i]->pipe_ptr->nwriter);
//             }
//         }
//     }

//     enqueue(&readyQueue, p);

//     kprintf("FORK: proc %d forked a child %d\n", running->pid, p->pid);
//     printQ(readyQueue);

//     return p->pid;
//     // now write exec, figure out how to hook it to status
//     // char *uline is passed, how?

// }