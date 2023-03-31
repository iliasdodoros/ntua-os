#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <assert.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include "proc-common.h"
#include "tree.h"

void fork_procs(struct tree_node *ptr)
{
    int i = 0;
    int status;
    pid_t childPid[ptr->nr_children];
    pid_t pid;
    // change name of the process from file
    change_pname(ptr->name);
    // create children
    for (i = 0; i < ptr->nr_children; i++)
    {
        childPid[i] = fork();
        // the children go in the if
        if (childPid[i] == 0)
        {
            change_pname((ptr->children + i)->name);
            // if it is a leaf it goes in the if
            if ((ptr->children + i)->nr_children == 0)
            {
                // the leafs stop
                raise(SIGSTOP);
                printf("%s\n", (ptr->children + i)->name);
                exit(0);
            }
            else
            {
                // if it is a parent it goes in the if to create all the children
                fork_procs(ptr->children + i);
            }
        }
    }

    raise(SIGSTOP);
    // wake up all the kids
    i = 0;
    for (i = 0; i < ptr->nr_children; i++)
    {
        kill(childPid[i], SIGCONT);
        // wait for the kids to finish
        pid = wait(&status);
        explain_wait_status(pid, status);
    }

    printf("%s\n", ptr->name);

    exit(0);
}

int main(int argc, char **argv)
{
    struct tree_node *root;
    pid_t pid;
    int status;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./ask3 [input file]\n");
        exit(1);
    }

    if (open(argv[1], O_RDONLY) < 0)
    {
        perror("Error opening");
    }

    root = get_tree_from_file(argv[1]);
    // create root
    pid = fork();
    if (pid < 0)
    {
        perror("Error first fork");
        exit(2);
    }
    // the child process goes in the if
    if (pid == 0)
    {
        fork_procs(root);
        exit(0);
    }
    // wait for all children to stop
    wait_for_ready_children(1);
    show_pstree(pid);
    // send continue signal to root
    kill(pid, SIGCONT);

    pid = wait(&status);
    explain_wait_status(pid, status);
    return 0;
}