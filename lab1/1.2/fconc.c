#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// WRITE
void doWrite(int fd, const char *buff, size_t len)
{
        size_t idx;
        ssize_t wcnt;
        idx = 0;
        do
        {
                wcnt = write(fd, buff + idx, len - idx);
                if (wcnt == -1)
                {
                        perror("write");
                        exit(1);
                }
                idx += wcnt;
        } while (idx < len);
}

void write_file(int fd, const char *infile)
{
        ssize_t rcnt = 0;
        int idx_read;
        int fd2;
        fd2 = open(infile, O_RDONLY);
        char buff[1024];
        if (fd2 == -1)
        {
                perror("open");
                exit(1);
        }
        //  Read until end of file

        for(;;)
        {

                rcnt = read(fd2, buff, sizeof(buff) - 1);
                if (rcnt == 0)
                {
                        break;
                }
                if (rcnt == -1)
                {
                        perror("read");
        }

                int len = strlen(buff);
                doWrite(fd, buff, rcnt);
        }

        close(fd2);
}

int main(int argc, char **argv)

{
        int oflags, mode;
        int fd;
        oflags = O_CREAT | O_WRONLY | O_TRUNC;
        mode = S_IRUSR | S_IWUSR;
        if (argc == 2) // just one input file
        {
                printf("Usage:./fconc infile1 infile2 [outfiledefault:fconc.out)]");
        }
        else if (argc == 3) // two input files
        {
                fd = open("fconc.out", oflags, mode);
        }

        else if (argc == 4)
                // two input files one output
        {
                fd = open(argv[3], oflags, mode);
        }

        if ((argc == 3) || (argc == 4))
        {

                if (fd == -1)
                {
                        perror("open");
                        exit(1);
                }
                write_file(fd, argv[1]);
                write_file(fd, argv[2]);
                close(fd);
        }
        return 0;
}
