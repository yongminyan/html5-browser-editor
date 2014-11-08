#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>

#include <string.h>
#include <syslog.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <assert.h>


#define PORT_LWS  9527
#define PORT_IPC  9521
#define DEBUG 1 


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int create_proc_as_user(int gid, int uid, char *argv[], char *envp[]) {
    pid_t pid = fork();
    if (0 == pid) { 
        setgid(gid);
        setuid(uid);
        int ret = execve(argv[0], &argv[0], envp);
        if (ret != 0) {
            perror("create_proc_as_user(), in child process");
            return ret;
        }
    } 
    else if (pid > 0) {
        // do nothing for now
    } 
    else {
        perror("create_proc_as_user, fork()");
        return pid; 
    }
    return 0;
}

int name_to_uid(char const *name, uid_t *login_uid, gid_t *login_gid)
{
    if (NULL == name || NULL == login_uid || NULL == login_gid)
        return -1;
    long const buflen = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (buflen == -1)
        return -1;
    // requires c99
    char buf[buflen];
    struct passwd pwbuf, *pwbufp;
    if (0 != getpwnam_r(name, &pwbuf, buf, buflen, &pwbufp)
            || !pwbufp)
        return -1;

    *login_uid = pwbufp->pw_uid;
    *login_gid = pwbufp->pw_gid;
    return 0; // success
}

#endif
