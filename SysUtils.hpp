#ifndef SYSUTILS_H
#define SYSUTILS_H

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <functional>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

typedef void (*sighandler_t)(int);

sighandler_t Signal(int signum, sighandler_t handler)
{
    struct sigaction action, old_action;
 
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);  /* Block sigs of type being handled */    
    action.sa_flags = SA_RESTART;  /* Restart syscalls if possible     */
    if (sigaction(signum, &action, &old_action) < 0) 
    {        
	perror("sigaction");
        exit(EXIT_FAILURE);
    }
    return (old_action.sa_handler);
}

void sig_safe_error(const char *msg)
{
    write(STDERR_FILENO, msg, strlen(msg));
    _exit(EXIT_FAILURE);
}

void sig_safe_print(const char *msg)
{
    write(STDOUT_FILENO, msg, strlen(msg));
}

// signal handler for SIGCHLD
void sigchld_handler(int sig)
{
    int olderrno = errno;
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0)
    {
	continue;
    }
    if (pid == -1 && errno != ECHILD)
    {
	sig_safe_error("waitpid error");
    }
    errno = olderrno;
}


#endif /* SYSUTILS_H */
