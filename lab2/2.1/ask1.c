#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "proc-common.h"

#define SLEEP_PROC_SEC 10
#define SLEEP_TREE_SEC 3

void fork_procs(void)
{

	pid_t pidB, pidC, pidD;
	int status;

	change_pname("A");
	printf("A created\n");
	pidB = fork();
	if (pidB < 0)
	{
		perror("creating B");
		exit(1);
	}
	if (pidB == 0)
	{
		// Node B
		change_pname("B");
		printf("B created\n");
		pidD = fork();
		if (pidD < 0)
		{
			perror("creating D");
			exit(1);
		}
		if (pidD == 0)
		{
			// Node D
			change_pname("D");
			printf("D created\n");
			printf("D sleeping\n");
			sleep(SLEEP_PROC_SEC);
			printf("D exiting\n");
			exit(13);
		}
		if (pidD > 0)
		{
			printf("B waiting\n");
			pidD = wait(&status);
			explain_wait_status(pidD, status);
			printf("B exiting\n");
			exit(19);
		}
	}
	if (pidB > 0)
	{
		// Node A
		pidC = fork();
		if (pidC < 0)
		{
			perror("creating C");
			exit(1);
		}
		if (pidC == 0)
		{
			// Node C
			change_pname("C");
			printf("C created\n");
			printf("C sleeping\n");
			sleep(SLEEP_PROC_SEC);
			printf("C exiting\n");
			exit(17);
		}
		if (pidC > 0)
		{
			// Node A
			int pid_done;
			printf("A waiting\n");
			pid_done = waitpid(-1, &status, 0);
			explain_wait_status(pid_done, status);
			pid_done = waitpid(-1, &status, 0);
			explain_wait_status(pid_done, status);
			printf("A exiting\n");
			exit(16);
		}
	}
}

int main(void)
{
	pid_t pid;
	int status;

	// Create root A
	pid = fork();
	if (pid < 0)
	{
		perror("first fork");
		exit(1);
	}
	if (pid == 0)
	{
		// Child
		fork_procs();
		exit(1);
	}

	sleep(SLEEP_TREE_SEC);

	show_pstree(pid);

	pid = wait(&status);
	explain_wait_status(pid, status);

	return 0;
}