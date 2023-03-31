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

#define SLEEP_TREE_SEC 6
#define SLEEP_LEAF 10

void forks(struct tree_node *ptr)
{
    int i = 0;
    int status;
    pid_t pid;
    //change process name from file
    change_pname(ptr->name);
    //create all children
    for (i = 0; i < ptr->nr_children; i++)
    {
        printf("i am %s and i have to create %d children\n", ptr->name, ptr->nr_children - i);
        int p = fork();
        //the children enter the if
        if (p == 0)
        {
            change_pname((ptr->children + i)->name);
            //the leafs enter the if
            if ((ptr->children + i)->nr_children == 0)
            {
                printf("i am %s and i am going to sleep\n", (ptr->children + i)->name);
                //the leafs sleep
                sleep(SLEEP_LEAF);
                printf("child %s woke up and exiting \n", (ptr->children + i)->name);
                exit(0);
            }
            else
            {
                forks(ptr->children + i);
            }
        }
    }
    //wait for all children to wake up and then exit
    for (i = 0; i < ptr->nr_children; i++)
    {
        pid = wait(&status);
        explain_wait_status(pid, status);
    }
    exit(0);
}


int main(int argc, char **argv)
{
    struct tree_node *root;
    int status;
    id_t pid;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./ask2 [input file]\n");
        exit(1);
    }
    //opening file
    if (open(argv[1], O_RDONLY) < 0)
    {
        perror("opening file");
    }
    //get root from file
    root = get_tree_from_file(argv[1]);
    //create the first child
    pid = fork();
    if (pid < 0)
    {
        perror("first fork");
        exit(1);
    }
    //the child process enters the if 
    if (pid == 0)
    {
        forks(root);
        exit(0);
    }

    sleep(SLEEP_TREE_SEC);
    show_pstree(pid);
    // fro all children to wake up 
    pid = wait(&status);
    explain_wait_status(pid, status);
    return 0;
}

