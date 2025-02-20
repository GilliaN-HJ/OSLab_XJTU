#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
    pid_t pid, pid1;

    pid = fork();

    if (pid < 0){
        fprintf (stderr, "Fork Failed");
        return 1;
    }

    else if (pid == 0){
        pid1 = getpid();
        printf("child PID = %d\n", pid1);

        // 使用 exec() 系列函数调用 system_call 程序
        char *args[] = {"./system_call", NULL};
        execvp(args[0], args);
        printf("child PID = %d\n", pid1);
    }

    else{
        pid1 = getpid();
        printf("parent PID = %d\n", pid1);
        wait(NULL);
    }

    return 0;
}