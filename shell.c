/*
 * shell.c
 * A tiny shell for Assignment 6. It launches programs normally; countnames
 * performs its own pthread-based parallelism.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXLINE 1024
#define MAX_ARGS 128

int main(void)
{
    char buf[MAXLINE];

    printf("%% ");
    while (fgets(buf, sizeof(buf), stdin) != NULL)
    {
        char *args[MAX_ARGS];
        int argc = 0;
        char *token;

        buf[strcspn(buf, "\n")] = '\0';
        token = strtok(buf, " \t");

        while (token != NULL && argc < MAX_ARGS - 1)
        {
            args[argc++] = token;
            token = strtok(NULL, " \t");
        }
        args[argc] = NULL;

        if (argc == 0)
        {
            printf("%% ");
            continue;
        }

        if (strcmp(args[0], "quit") == 0 || strcmp(args[0], "exit") == 0)
        {
            break;
        }

        {
            pid_t pid = fork();
            int status = 0;

            if (pid < 0)
            {
                perror("fork");
            }
            else if (pid == 0)
            {
                execvp(args[0], args);
                perror("exec");
                _exit(127);
            }
            else
            {
                waitpid(pid, &status, 0);
            }
        }

        printf("%% ");
    }

    return 0;
}
