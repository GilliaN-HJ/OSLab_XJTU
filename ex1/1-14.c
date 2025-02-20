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
        printf("child: value = %d  ", value);
        printf("child: *value = %p  \n", p);
    }

    else{
        pid1 = getpid();
        value = 3;
        printf("parent: value = %d  ", value);
        printf("parent: *value = %p  \n", p);
        wait(NULL);
    }

    // value++
    value += 1;
    printf("before return: value = %d  ", value);
    printf("before return: *value = %p  \n", p);

    return 0;
}