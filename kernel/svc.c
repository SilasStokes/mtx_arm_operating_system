/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

#include "../type.h"

// missing functions:
int vfork(){printf("kernel: vfork not yet implemented\n"); }
int sbrk(){ }
int page_out(){ }
int getesp(){ }
int ptable(){ }
int kitimer();
int kcdSector(){ }
int do_cmd(){ }
int kcolor(){ }

/*********************************************************************
                    syscall routing table
*********************************************************************/
extern int 
getpid(), getppid(), getpri(), ksetpri(), getuid(), kchuid(), kswitch(),
fork(), exec(), kwait(), vfork(),  kthread(), kmutex_creat(), kmutex_lock(), 
kmutex_unlock(), kmutex_destroy(), kmkdir(), krmdir(), kcreat(), klink(), 
kunlink(), ksymlink(), kreadlink(), kchdir(), kgetcwd(), kstat(), kfstat(), 
kopen(), kclose(), klseek(), kread(), kwrite(), kpipe(), kchmod(), kchown(), 
ktouch(), ksettty(), kgettty(), kdup(), kdup2(), kps(), kmount(), kumount(), 
kcdSector(), do_cmd(), kkill(), ksignal(), kpause(), kitimer(), ksend(), 
krecv(), ktjoin(), ktexit(), khits(), kcolor(), ksync(), khits(), kexit(),
kgetPaddress(), thinit(), sbrk(), page_out(), ptable();
int getphypage(int *y, int *z){ }

extern int nocall();
  //fork,     kexec,     kwait,     kgetPaddress,  kthread, 
int (*f[ ])() = {
//0         1          2          3         4
  getpid,   getppid,   getpri,    ksetpri,  getuid,
//5         6           7           8       9 
  kchuid,   kswitch,   nocall,    nocall,   kexit,

//10        11      12              13      14  
  fork,     exec,      kwait,     vfork,    kthread, 
//15            16          17              18              19
  kmutex_creat, kmutex_lock, kmutex_unlock, kmutex_destroy, nocall,
  
//20        21          22          23      24
  kmkdir,   krmdir,    kcreat,    klink,    kunlink, 
//25        26          27          28      29
  ksymlink, kreadlink, kchdir,    kgetcwd,  kstat, 

//30        31      32              33      34
  kfstat,   kopen,     kclose,    klseek,   kread, 
//35        36          37          38      39
  kwrite,   kpipe,     kchmod,    kchown,   ktouch,

//40        41          42      43          44
  ksettty,  kgettty,   kdup,      kdup2,    kps, 
// 45       46          47          48      49
  kmount,   kumount,   kcdSector, do_cmd,   nocall, 

//50        51          52          53      54  
  kkill,    ksignal,   kpause,    kitimer,  ksend, 
//55        56          57          58      59  
  krecv,    ktjoin,    ktexit,    khits,    kcolor,   

//60        61          62          63      64
  ksync,    kps,       thinit,    sbrk,     page_out,
//65                    66
  getphypage,ptable,   kgetPaddress
}; 

/***************************************************************************
ARM: syscall = swi # in Umode ==> SVC vector: LDR PC svc_handler_addr (svc_enty)
=> CPU executes from svc_entry in ts.s is SVC mode: stack=PROC's (empty) kstack

svc_entry: 
1. save r0-r12,lr in SVC stack:  stmfd sp!, {r0-r12, lr}
2. save Umode sp, lr cpsr into PROC.usp, upc, cpsr
3. save Kmode sp into PROC.ksp (used in fork() to copy parent's kstack)
4. enable IRQ, FIQ interrupts
5. call svc_handler() in C 
     BL svc_handler;             syscall params a,b,c,d are in r0-r3

goUmode:
1. mask out IRQ FIQ interrupts
2. restore Umode usp, spcr 
3. replace saved R0 in kstack with r from syscall function

4. pop saved regs from kstack BUT also copy spsr into cpsr ==> back to Umode
   ldmfd sp!, {r0-r12, pc}^   // ^ : pop kstack AND to previous mode
***************************************************************************/

int svc_handler(volatile int a, int b, int c, int d) 
{
  int r;
  //  printf("proc%d svc call %d\n", running->pid, a);
  
  if (running->res->signal)
     kpsig();

  unlock();
 
  if (a > 67){ // number of syscalls
    printf("invlaid syscall no=%d\n", a);
    return -1;
  }

  r = (*f[a])(b, c, d);     // call the syscall function

  if (running->res->signal){  // check and handle signal
      printf("%d has signal ", running->pid);
      kpsig();
  }

  return r;  // return to ts.s ==> replace R0 in kstack as r to Umode
}
