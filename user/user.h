#ifndef USER_H
#define USER_H

#include "debug.h"

// type stuff:
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

// globals from crt0.c

// Types from ../type.h

typedef struct semaphore{
  int value;
  struct proc *queue;
}SEMAPHORE;


typedef struct ext2_inode {
	u16	i_mode;		/* File mode */
	u16	i_uid;		/* Owner Uid */
	u32	i_size;		/* Size in bytes */
	u32	i_atime;	/* Access time */
	u32	i_ctime;	/* Creation time */
	u32	i_mtime;	/* Modification time */
	u32	i_dtime;	/* Deletion Time */
	u16	i_gid;		/* Group Id */
	u16	i_links_count;	/* Links count */
	u32	i_blocks;	/* Blocks count */
	u32	i_flags;	/* File flags */
        u32     dummy;
	u32	i_block[15];    /* Pointers to blocks */
        u32     pad[5];         /* inode size MUST be 128 bytes */
        u32	i_date;         /* MTX date */
	u32	i_time;         /* MTX time */
}INODE;

/******************** KCW on MINODE *******************
 refCount = # of procs using this MINODE
 lock is a semaphore for exclusive access to this MINODE
              WHY lock? 
 When a proc issues disk I/O on a MINODE, it may gets 
 blocked while waiting for disk completion interrupt =>
 another proc may find the same MINODE and proceed to 
 use it (before fully loaded) OR (even worse!) modify it.
 Hence the need of a lock.
 Unix uses a "locked" flag to sleep/wakeup. 
 MTX uses a semaphore in each MINODE
*****************************************************/
typedef struct Minode{		
  INODE    INODE; 
  int      dev, ino;
  int      refCount;
  int      dirty;
  int      mounted;
  struct Mount *mountptr;
  struct semaphore lock;  // locking semaphore
} MINODE;

typedef struct Mount{
        int    ninodes;
        int    nblocks;
        int    dev, busy;   
        struct Minode *mounted_inode;
        char   name[32]; 
        char   mount_name[32];
        // mounted dev's map & inodes_block numbers
        // although all EXT2 FS, these values may be different PER device
        int    BMAP,IMAP,IBLOCK; 
} MOUNT;

typedef struct Oft{
  int   mode;
  int   refCount;
  struct Minode *inodeptr;
  struct pipe *pipe_ptr;
  u32  offset;
  char  name[32];
} OFT;

typedef struct pipe{
  char  buf[128];
  int   head, tail, data, room;
  int   nreader, nwriter;
  int   busy;
}PIPE;

// Types from uio.c
typedef struct ext2_dir_entry_2
{
    u32 inode;   /* Inode number */
    u16 rec_len; /* Directory entry length */
    u8 name_len; /* Name length */
    u8 file_type;
    char name[255]; /* File name */
} DIR;

typedef struct stat
{
    u16 st_dev;   /* major/minor device number */
    u16 st_ino;   /* i-node number */
    u16 st_mode;  /* file mode, protection bits, etc. */
    u16 st_nlink; /* # links; TEMPORARY HACK: should be nlink_t*/
    u16 st_uid;   /* uid of the file's owner */
    u16 st_gid;   /* gid; TEMPORARY HACK: should be gid_t */
    u16 st_rdev;
    long st_size;  /* file size */
    long st_atime; /* time of last access */
    long st_mtime; // time of last modification
    long st_ctime; // time of creation
    long st_dtime;
    long st_date;
    long st_time;
} STAT;

// UNIX <fcntl.h> constants: <asm/fcntl.h> in Linux
#define O_RDONLY 00
#define O_WRONLY 01
#define O_RDWR 02
#define O_CREAT 0100  /* not fcntl */
#define O_TRUNC 01000 /* not fcntl */
#define O_APPEND 02000

#define ST_TERMINAL 0x21A4
#define ST_TERMINAL_IN 0xF018
#define PIPE_MODE 0xF018
#define ST_FILE     0x8124
#define ST_DIR      0x41ED
#define MAX_LINE_LENGTH 128

#define SIGINT 2
#define IGNORE 1

#define DIR_MODE          0x41ED
// #define DIR_MODE          0040777 // 0x41ED;
#define FILE_MODE         0100644

#define bool int
#define true 1
#define false 0

// #define EOF -1 // already in ucode.c globals

// #define exit mexit


// globals from ucode.c
int pid;
char line[64], pathname[32], i2[32], i3[32];
char *name[16], components[64];
int nk;
#define EOF -1
extern char cr;

// globals from uio.c
char space = ' ';
char *ctable = "0123456789ABCDEF";
char cr = '\r';

// forward declaring main for crt0.c
int main(int argc, char *argv[]);

// globals from crt0.c
// int argc;
// char *argv[32];
// crt0.c
void token(char *line);
void showarg(int argc, char *argv[]);
void main0(char *s);

// us.s
extern int u_entry();
extern int syscall();
extern int getmpsp();
extern int getulr();
extern int getFP();
extern int getusp();
extern int getcsr();
extern int getmyaddress();
extern int setjmp();
extern int longjmp();


// // string.c
// bcopy, bzero had to be renamed bc they conflicted with standard c functions and I was trying to compile w/o warnings.
int kc_bcopy(const void *src, void *dest, unsigned int n);
int kc_bzero(void *dest, unsigned int n);
char *memcpy(void *dest, const void *src, unsigned int n);
char *memset(void *dest, int c, unsigned int n);
int memcmp(const void *s1, const void *s2, unsigned int n);
int strcmp(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
int strlen(const char *s);
int strcat(char *s1, char *s2);
int strncpy(char *s1, char *s2, int n);
int strncmp(char *s1, char *s2, int n);
char *strstr(char *s1, char *s2);
int setzero(char *dst, int size);
void delay();
int copy(char *dest, char *src);
int atoi(char *s);

// ucode.c
void putchar(const char c);
int getc();
// getline() does NOT show the input chars AND no cooking: 
// for reditected inputs from a file which may contain '\b' chars
int getline(char *s);
// gets() show each input char AND cook input line
int getc_file(int fd);
int getline_file(char *, int fd);

int gets(char *s);
int fe(char *cmd);
int getpid();
int getppid();
void chname();
int getpri();
int chpri(int value);
int getuid();
int chuid(int uid, int gid);
int tswitch();
int fork();
int exec(char *cmd_line);
int wait(int *status);
int vfork();
int thread(int fn, int stack, int ptr);
int mutex_creat();
int mutex_lock(int *m);
int mutex_unlock(int *m);
int mutex_destroy(int *m);
int mkdir(char *name);
int rmdir(char *name);
int creat(char *filename);
int link(char *oldfile, char *newfile);
int unlink(char *file);
int symlink(char *oldfile, char *newfile);
int readlink(char *file, char *linkname);
int chdir(char *name);
int getcwd(char *cwdname);
int stat(char *filename, struct stat *sPtr);
int fstat(int fd, char *sptr);
int open(char *file, int flag);
int close(int fd);
int lseek(int fd, u32 offset, int ww);
int read(int fd, char *buf, int nbytes);
int get_block(int dev, int blk, char * buf);
int write(int fd, char *buf, int nbytes);
int pipe(int *pd);
int chmod(char *file, u16 mode);
int chown(char *file, int uid);
int touch(char *filename);
int settty(char *tty);
int gettty(char *tty);
int dup(int fd);
int dup2(int fd, int gd);
int mount(char *dev, char **mpt);
int umount(char *dev);
int getSector(u32 sector, char *ubuf, u16 nsector);
int do_cmd(int cmd, u16 value);
int kill(int sig, int pid);
int signal(int sig, int catcher);
int pause(int t);
int itimer(int t);
int send(char *msg, int pid);
int recv(char *msg);
int do_texit();
int tjoin(int n);
int texit(int v);
/************************************************
int ps(char *y)
{
   return syscall(44,y,0);
}
// ********** CDROM syscalls ******************
int setcolor(int color)
{
  return syscall(59, color,0);
}
*************************************/
int sync();
int ups();
int thinit();
int sbrk();
int page_out(int n);
int getphypage(int x, int y);
int pagetable();
int getcs();
int exit(int value);
int uexit();
int pwd();
int eatpath(char *line, char *name[ ]) ;
int strcasecmp(char *s1, char *s2);


// ## uio.c
int puts(const char *s);
int kc_putc(char c);
int puts_nonewline(char *s);

int putc_nonewline(char c);

void prints(char *s);
void mputs(char *s);
void print2f(char *s);
void rpi(int x);
void printi(int x);
void rpu(u32 x);
void printu(u32 x);
void rpx(u32 x);
void printx(u32 x);
void printc(char c);
int printf(char *fmt, ...);



#endif