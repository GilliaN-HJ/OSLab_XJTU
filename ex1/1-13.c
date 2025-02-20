#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

int value = 0;
int* p = &value;

int main(){
    pid_t pid, pid1;

    pid = fork();

    if (pid < 0){
        fprintf (stderr, "Fork Failed");
        return 1;
    }

    else if (pid == 0){
        pid1 = getpid();
        value = 1;
        //printf("child: pid = %d  ", pid);
        //printf("child: pid1 = %d  ", pid1);
        printf("child: global value = %d  ", value);
        printf("child: *value = %p  ", p);
    }

    else{
        pid1 = getpid();
        value = 3;
        //printf("parent: pid = %d  ", pid);
        //printf("parent: pid1 = %d  ", pid1);
        printf("parent: global value = %d  ", value);
        printf("parent: *value = %p  ", p);
        wait(NULL);
    }

    return 0;
}