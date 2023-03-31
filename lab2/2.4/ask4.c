#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "tree.h"
#include "proc-common.h"

#define SLEEP_TREE_SEC 1
#define SLEEP_LEAF 1
// function to calculate the results from the values sent from children
char calculate(char kind, char a, char b)
{
	int a_int = a - '0', b_int = b - '0';
	switch (kind)
	{
	case '+':
	{
		printf("result: %i\n", a_int + b_int);
		return a_int + b_int + '0';
	}
	case '*':
	{
		printf("result: %i\n", a_int * b_int);
		return a_int * b_int + '0';
	}
	}
	return -1;
}

void fork_procs(struct tree_node *root, int fd)
{
	int i;
	

	printf("PID = %ld, name %s, starting\n",
		   (long)getpid(), root->name);

	change_pname(root->name);
	// leafs enter the if
	if (root->nr_children == 0)
	{
		printf("leaf %s created\n", root->name);
		// convert the name of the node to write it to the pipe
		sleep(SLEEP_LEAF);
		char cur = atoi(root->name) + '0';
		if (write(fd, &cur, sizeof(cur)) != sizeof(cur))
		{
			perror("Child: write to pipe");
			exit(1);
		}
		printf("PID = %ld, name = %s is awake and wrote to fd \n", (long)getpid(), root->name);
		exit(10);
	}

	printf("%s created\n", root->name);
	// save children pid and create the pipes between parents and children
	pid_t pid_child[root->nr_children];
	int pfd_child[2];
	printf("parent creating pipe...\n");
	if (pipe(pfd_child) < 0)
	{
		perror("pipe");
		exit(1);
	}

	// create different pipes for each child and give the child the write end of the pipe
	for (i = 0; i < root->nr_children; i++)
	{
		pid_child[i] = fork();
		if (pid_child[i] == 0)
		{
			// close the read end of the pipe in the children processes
			close(pfd_child[0]);
			fork_procs(root->children + i, pfd_child[1]);
			exit(0);
		}
	}
	// close write end of the pipe of the parent processes
	close(pfd_child[1]);

	// stop until child sends their name

	printf("PID = %ld, name = %s is awake\n",(long)getpid(), root->name);
	char val[2];
	for (i = 0; i < root->nr_children; i++)
	{

		// read from the pipe
		if (read(pfd_child[0], &val[i], sizeof(val[i])) != sizeof(val[i]))
		{
			perror("child: read from pipe");
			exit(1);
		}
		printf("parent received value from the pipe\n");
		
	}

	char result = calculate(*root->name, val[0], val[1]);
	// send the parent result we have from the calculate function
	if (write(fd, &result, sizeof(result)) != sizeof(result))
	{
		
		perror("Child: write to pipe");
		exit(1);
	}
	exit(0);
}

int main(int argc, char **argv)
{
	int pfd[2];
	pid_t pid;

	struct tree_node *root;
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <input_tree_file>\n\n", argv[0]);
		exit(1);
	}
	root = get_tree_from_file(argv[1]);

	printf("Parent: Creating pipe...\n");
	// create first pipe
	if (pipe(pfd) < 0)
	{
		perror("pipe");
		exit(1);
	}
	// create first child process
	pid = fork();

	if (pid < 0)
	{
		perror("main:fork");
		exit(1);
	}
	
	if (pid == 0)
	{
		fork_procs(root, pfd[1]);

		exit(1);
	}

	// display the process tree
    show_pstree(pid);
	// parent sleeps so that the children finish first
	sleep(SLEEP_TREE_SEC);

	printf("parent process done\n");
	return 0;
}