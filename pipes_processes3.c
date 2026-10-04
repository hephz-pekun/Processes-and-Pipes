#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/*
 * pipes_processes3.c

 * Pipe 1 transfers data from cat to grep.
 * Pipe 2 transfers data from grep to sort.
 */



int main(int argc, char **argv)
{
    int pipe1[2];   /* cat -> grep */
    int pipe2[2];   /* grep -> sort */

    int pid1;
    int pid2;

    char *cat_args[]  = {"cat", "scores", NULL};
    char *grep_args[] = {"grep", argv[1], NULL};
    char *sort_args[] = {"sort", NULL};

    /* Verify command-line argument */
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <grep_pattern>\n", argv[0]);
        exit(1);
    }

    /* Create both pipes */
    if (pipe(pipe1) == -1)
    {
        perror("pipe1");
        exit(1);
    }

    if (pipe(pipe2) == -1)
    {
        perror("pipe2");
        exit(1);
    }

    /* Create child process (grep) */
    pid1 = fork();

    if (pid1 < 0)
    {
        perror("fork");
        exit(1);
    }

    if (pid1 == 0)
    {
        /* Create grandchild process (sort) */
        pid2 = fork();

        if (pid2 < 0)
        {
            perror("fork");
            exit(1);
        }

        if (pid2 == 0)
        {
            /*
             * P3: SORT PROCESS
             * Reads from pipe2 and writes to terminal.
             */

            dup2(pipe2[0], STDIN_FILENO);

            close(pipe2[1]);
            close(pipe1[0]);
            close(pipe1[1]);
            close(pipe2[0]);

            execvp("sort", sort_args);

            perror("execvp sort");
            exit(1);
        }
        else
        {
            /*
             * P2: GREP PROCESS
             * Reads from pipe1 and writes to pipe2.
             */

            dup2(pipe1[0], STDIN_FILENO);
            dup2(pipe2[1], STDOUT_FILENO);

            close(pipe1[1]);
            close(pipe2[0]);
            close(pipe1[0]);
            close(pipe2[1]);

            execvp("grep", grep_args);

            perror("execvp grep");
            exit(1);
        }
    }
    else
    {
        /*
         * P1: CAT PROCESS
         * Reads scores file and writes to pipe1.
         */

        dup2(pipe1[1], STDOUT_FILENO);

        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);

        execvp("cat", cat_args);

        perror("execvp cat");
        exit(1);
    }

    return 0;
}