#include "user.h"
#include "ucode.c"
#include "uio.c"
#include "string.c"
#include "crt0.c"

// globals
STAT statstruct;

char buf[1024];
char path[128];
char *dest;

void myls_file(char *abspath, char *name)
{
    char fullpath[64];
    STAT lclstat;
    strcpy(fullpath, abspath);
    strcat(fullpath, "/");
    strcat(fullpath, name);
    // debug("calling lstat on %s\n", fullpath);
    stat(fullpath, &lclstat);
    // debug("called stat on %s, st_mode = %u\n", fullpath, (unsigned int) lclstat.st_mode);
    // printf("")
    // char ftime[64];
    int i;
    char *t1 = "xwrxwrxwr-------";
    char *t2 = "----------------";

    // statstruct.st_mode
    if ((lclstat.st_mode & 0xF000) == 0x8000) // if (S_ISREG())
        kc_putc('-');
        // printf(" %c", '-');
    if ((lclstat.st_mode & 0xF000) == 0x4000) // if (S_ISDIR())
        kc_putc('d');
        // printf(" %c", 'd');
    if ((lclstat.st_mode & 0xF000) == 0xA000) // if (S_ISLNK())
        kc_putc('l');
        // printf(" %c", 'l');
    for (i = 8; i >= 0; i--)
    {
        if (lclstat.st_mode & (1 << i)) // print r|w|x
            // printf(" %c", t1[i]);
            kc_putc(t1[i]);
        else
            kc_putc(t2[i]); // or print -
    }
    printf("  ");                             // file permissions
    printf("%d  ", lclstat.st_nlink); // number of hard links
    printf("%d  ", lclstat.st_uid);       // owner name
    printf("%d  ", lclstat.st_gid);         // owner group

    printf("%d  ", lclstat.st_size); // file size in bytes

    printf("%s\n", name);           // name


}

void myls_dir(char *dirpath)
{

    debug("in myls_dir with %s\n", dirpath);
    char temp[64];
    printf("ino permissions links owner group bytes time name\n");
    int fd = open(dirpath, O_RDONLY);
    if (fd < 1)
    {
        printf("CANT OPEN PATH: %s\n", dirpath);
        exit(1);
    }
    debug("opened file desc: %d\n", fd);

    int i = 0;
    char *cp;
    DIR *dp;
    int num_bytes = read(fd, buf, 1024);
    debug("num_bytes: %d\n", num_bytes);
    while( num_bytes > 0)
    {
        cp = buf;
        dp = (DIR *)cp;
        while (cp < buf + 1024) {
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = 0;
            // printf("%s\n", temp);
            myls_file(dirpath, temp);
            cp += dp->rec_len;
            dp = (DIR *)cp;
        }
        num_bytes = read(fd, buf, 1024);
    }
    // debug("exiting myls_dir\n");
}

// expected input:
// argv[0] = "si_ls"
// argv[1] =  dest
int main(int argc, char *argv[])
{
    int i;
    if (argv[1] == 0)
    {
        // ls cwd
        int n = getcwd(path);
        debug("n = %d, cwd = %s", n, path);
        myls_dir(path);
        debug("done myls_dir cwd\n");
        exit(0);
    }
    dest = argv[1];
    if (*dest == '/') { // if absolute path
        STAT st;
        stat(dest, &st);
        if ((st.st_mode & 0xF000 )== 0x8000){
            char *f, *d;
            char *cp;
            int index = 0;
            for (i = 0; *cp != 0; i++) {
                if (*cp == '/'){
                    index = i;
                }
                cp++;
            }
            d = dest;
            dest[index] = 0;
            f = dest[index + 1];
            debug("calling myls_file with %s and %s\n", d, f);
            myls_file(d, f);
        } else {
            debug("calling ls_dir from absolute path: %s\n", dest);
            myls_dir(dest);
        }

    } else {
        printf("Relative dest\n");
        int n = getcwd(path);
        strcat(path, "/");
        strcat(path, dest);
        myls_dir(path);
        // get cwd
        // append location to it.
    }

    // printf("INSIDE SI_LS. Exiting...\n");
    // exit(0);
}

int main1(int argc, char *argv[])
{
    // char *dest = "/etc/passwd";
    char *dest = "/";
    printf("ino permissions links owner group bytes time name\n");
    int fd = open("/", O_RDONLY);

    printf("file descriptor: %d\n", fd);
    if (fd < 1)
    {
        printf("CANT OPEN PATH: %s\n", dest);
        exit(1);
    }

    char fchar;
    int bytes_read = read(fd, buf, 1024);
    printf("bytes_read: %d\n", bytes_read);
    printf("press enter:");
    getc();
    char *cp = buf;
    for (int i = 0; i < bytes_read; i++)
    {
        printf("char cp = %c\n", *cp);
        cp++;
    }

    // while (bytes_read > 0) {
    //     printf("char: %c ,", fchar);
    //     lseek(fd, 0, 1);
    //     bytes_read = read(fd, &fchar, 1);
    //     // printf("bytes_read: %d\n", bytes_read);
    // }
    printf("Done\n");
    // int i = 0;
    // int num_bytes = read(fd, buf, 1024);
    // debug("num_bytes: %d\n");
    // char * cp = buf;
    // debug("printing contents of dest: \n");
    // // while(*cp) {
    // for(i = 0; i < 1024; i ++) {
    //     printf("%c", *cp);
    //     cp++;
    // }
    // debug("done\n");
}