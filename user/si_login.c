
#include "user.h"
#include "ucode.c"
#include "uio.c"
#include "string.c"
#include "crt0.c"

char buf[1024], username[64], password[64];
char pline[64], mytty[64], *pname[8];
char *cp, *cq, *cpp, *cqq;
int gid, uid, i, n;

// seems stdin,stdout,stderr are keywords in bcc
// int stdin, stdout, stderr; ==> auto_start symbol
int in, out, err;
int fd;
// login : call by init as "login tty0" | "login ttyS0" | ...
// Upon entry, name[0]=filename, name[1]=mytty string
char whattty[64];
// username    passwd      uid     gid     extra info    home directory    command shell
// root        12345:      0   :   0       :super user:  /:                sh
// kcw:abcde:1:1:kcw user:/user/kcw:sh
// si_sh:silas:1:1:silas user:/user/silas:si_sh
// silas:silas:1:1:silas user:/user/silas:sh
// guest:mercy:2:2:guest user:/user/guest:sh
// buster:123:3:3:buster dog:/user/buster:sh
enum stdio {stdin, stdout, stderr};

typedef struct user {
    char username[64];
    char password[64];
    int uid;
    int gid;
    char userinfo[64];
    char homedir[64];
    char shell[64];
} USER;

#define NUM_USERS 8
USER USERS[NUM_USERS];

void user_init() {

    int i = 0, j = 0;
    USER * user;
    for (i = 0; i < NUM_USERS; i++) {
        user = &USERS[i];
        for (j = 0; j < 64; j++) {
            user-> username[j] = 0;
            user-> password[j] = 0;
            user-> userinfo[j] = 0;
            user-> homedir[j] = 0;
            user-> shell[j] = 0;
        }
        user -> uid = 0;
        user -> gid = 0;
    }
}

enum passwdfields {USERNAME, PASSWORD, GID, UID, USERINFO, HOMEDIR, SHELL};
int printASCIIvalues(char *s){
    printf(" [");
    while (*s){
        printf("%d", (int)*s);
        s++;
    }
    printf("]\n");
}
int main(int argc, char *argv[]) {
    int i = 0;
    if (argc < 2) {
        // no console file passed
        exit(1); /// which exit code should I use??
    }
    // (1). close file descriptors 0,1 inherited from INIT
    close(0); close(1); // close(2); // close all the inherited file descriptors
    // (2). open argv[1] 3 times for in out and err
    in = open(argv[1], O_RDONLY);
    out = open(argv[1], O_WRONLY);
    err = open(argv[1], O_WRONLY);
    if (in < 0 || out < 0 || err < 0) {
        printf("error opening tty io\n");
        exit(1);
    }
    
    // (3). settty(argv[1]) -> set tty name string in PROC.tty
    settty(argv[1]);

    // (4). open /etc/passwd for read
    int passwd_file = open("/etc/passwd", O_RDONLY);
    if (passwd_file < 0) {
        printf("Opening password file failed \n");
        exit(1);
    }

    // printf("ENTERED SI LOGIN: Calling user_init()\n");
    user_init();
    int bytes_read = 0;
    int num_newLines = 0;
    int passwd_field_index = 0;
    int string_index = 0;
    USER * u = &USERS[num_newLines];
    char * cp;
    char pw_char;
    char temp[2] = {'\0', '\0'};
    // printf("Entering while loop\n");
    bytes_read = read(passwd_file, &pw_char, 1);
    while (bytes_read > 0) {
        switch (pw_char){
            case ':':
                passwd_field_index ++;
                string_index = 0;
                break;
            case '\n':
                passwd_field_index = 0;
                string_index = 0;
                u = &USERS[++num_newLines];
                break;
            default :
                switch (passwd_field_index){
                    case USERNAME:
                        u -> username[string_index] = pw_char;
                        string_index++;
                        break;
                    case PASSWORD:
                        u -> password[string_index] = pw_char;
                        string_index ++;
                        break;
                    case GID:
                        temp[0] = pw_char;
                        u -> gid = atoi(temp);
                        break;
                    case UID:
                        temp[0] = pw_char;
                        u -> uid = atoi(temp);
                        break;
                    case USERINFO:
                        u -> userinfo[string_index] = pw_char;
                        string_index ++;
                        break;
                    case HOMEDIR:
                        u -> homedir[string_index] = pw_char;
                        string_index ++;
                        break;
                    case SHELL:
                        u -> shell[string_index] = pw_char;
                        string_index ++;
                        break;
                    default: 
                        break;
                }
            break;
        }
        lseek(passwd_file, 0, 1);
        bytes_read = read(passwd_file, &pw_char, 1);
    } 

    close(passwd_file);
    int incorrect = 1;
    while (incorrect) {
        printf("si_login: "); gets(username);
        printf("si_password: "); gets(password);
        for (i = 0; i < num_newLines; i ++) {
            if (strcmp(username, USERS[i].username) == 0 && strcmp(password, USERS[i].password) == 0 ){
                chuid(USERS[i].uid, USERS[i].gid);
                chdir(USERS[i].homedir);
                incorrect = 0;
                break;
            }
        }
        if (incorrect)
            printf("Username or Password not found, try again...\n");
    }
    
    

    while(1) {
        int status;
        exec("si_sh start");
    }
    // while(1) {
    //     // (5). get username and password
    //     printf("si_login: "); gets(username);
    //     printf("si_password: "); gets(password);
    //     for (i = 0; i < num_newLines; i ++) {
    //         if (strcmp(username, USERS[i].username) == 0 && strcmp(password, USERS[i].password) == 0 ){
    //             chuid(USERS[i].uid, USERS[i].gid);
    //             chdir(USERS[i].homedir);
    //             // exec(USERS[i].shell);
    //             int status;
    //                 while(1) {
    //                     exec("si_sh start");
    //                 }
    //             // if (fork()){
    //             //     wait(&status);
    //             //     continue;
    //             // } else {
    //             //     while(1) {
    //             //         exec("si_sh start");
    //             //     }
    //             // }
    //         }
    //     }
    //     printf("Username or Password not found, try again...\n");
    // }
    return 0;
}
