#include <stdio.h>
#include <unistd.h>

void zing() {
        char *user;
        user = getlogin();
        printf("Bye %s.\n",user);
}