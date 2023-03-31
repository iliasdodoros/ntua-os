/*
 * mandel.c
 *
 * A program to draw the Mandelbrot Set on a 256-color xterm.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <semaphore.h>
#include "mandel-lib.h"

#define MANDEL_MAX_ITERATION 100000

/***************************
 * Compile-time parameters *
 ***************************/

/*
 * Output at the terminal is is x_chars wide by y_chars long
 */
int y_chars = 50;
int x_chars = 90;

/*
 * The part of the complex plane to be drawn:
 * upper left corner is (xmin, ymax), lower right corner is (xmax, ymin)
 */
double xmin = -1.8, xmax = 1.0;
double ymin = -1.0, ymax = 1.0;

/*
 * Every character in the final output is
 * xstep x ystep units wide on the complex plane.
 */
double xstep;
double ystep;
sem_t *mutex;

struct thread_stats_struct
{
	pthread_t tid;
	int *color_val;
	int thrid;
	int thrcnt;
};

int my_atoi(char *s, int *val)
{
	long l;
	char *endp;
	l = strtol(s, &endp, 10);
	if (s != endp && *endp == '\0')
	{
		*val = l;
		return 0;
	}
	else
		return -1;
}

void *my_malloc(size_t size)
{
	void *p;
	if ((p = malloc(size)) == NULL)
	{
		fprintf(stderr, "Out of memory, failed to allocate %zd bytes\n",
				size);
		exit(1);
	}

	return p;
}

/*
 * This function computes a line of output
 * as an array of x_char color values.
 */
void compute_mandel_line(int line, int color_val[])
{
	/*
	 * x and y traverse the complex plane.
	 */
	double x, y;

	int n;
	int val;

	/* Find out the y value corresponding to this line */
	y = ymax - ystep * line;

	/* and iterate for all points on this line */
	for (x = xmin, n = 0; n < x_chars; x += xstep, n++)
	{

		/* Compute the point's color value */
		val = mandel_iterations_at_point(x, y, MANDEL_MAX_ITERATION);
		if (val > 255)
			val = 255;

		/* And store it in the color_val[] array */
		val = xterm_color(val);
		color_val[n] = val;
	}
}

/*
 * This function outputs an array of x_char color values
 * to a 256-color xterm.
 */
void output_mandel_line(int fd, int color_val[])
{
	int i;

	char point = '$';
	char newline = '\n';

	for (i = 0; i < x_chars; i++)
	{
		/* Set the current color, then output the point */
		set_xterm_color(fd, color_val[i]);
		if (write(fd, &point, 1) != 1)
		{
			perror("compute_and_output_mandel_line: write point");
			exit(1);
		}
	}

	/* Now that the line is done, output a newline character */
	if (write(fd, &newline, 1) != 1)
	{
		perror("compute_and_output_mandel_line: write newline");
		exit(1);
	}
}

void *compute_and_output_mandel_line(void *arg)
{
	struct thread_stats_struct *thread = arg;
	int i;

	for (i = thread->thrid; i < y_chars; i += thread->thrcnt)
	{
		compute_mandel_line(i, thread->color_val);
		sem_wait(&mutex[i % thread->thrcnt]);
		output_mandel_line(1, thread->color_val);
		sem_post(&mutex[(i + 1) % thread->thrcnt]);
	}

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s thread_count array_size\n\n"
						"Exactly one argument required:\n"
						"    thread_count: The number of threads to create.\n",
				argv[0]);
	}

	int line;
	int i, ret, thrcnt;
	struct thread_stats_struct *thread;

	xstep = (xmax - xmin) / x_chars;
	ystep = (ymax - ymin) / y_chars;

	if (my_atoi(argv[1], &thrcnt) < 0 || thrcnt <= 0)
	{
		fprintf(stderr, "`%s' is not valid for `thread_count'\n", argv[1]);
		exit(1);
	}

	thread = my_malloc(thrcnt * sizeof(*thread));
	mutex = my_malloc(thrcnt * sizeof(sem_t));
	for (i = 0; i < thrcnt; i++)
	{
		/* Initialize per-thread structure */
		thread[i].thrid = i;
		thread[i].thrcnt = thrcnt;
		thread[i].color_val = my_malloc(x_chars * sizeof(int));
		(i == 0) ? sem_init(&mutex[i], 0, 1) : sem_init(&mutex[i], 0, 0);
		/* Spawn new thread */
		ret = pthread_create(&thread[i].tid, NULL, compute_and_output_mandel_line, &thread[i]);
	}
	for (i = 0; i < thrcnt; i++)
	{
		ret = pthread_join(thread[i].tid, NULL);
		if (ret)
		{
			//		perror_pthread(ret, "pthread_join");
			exit(1);
		}
	}
	sem_destroy(&mutex);
	reset_xterm_color(1);
	return 0;
}
