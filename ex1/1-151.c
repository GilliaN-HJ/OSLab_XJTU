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

        // 使用 system() 调用外部命令执行 system_call 程序
        system("./system_call");
        printf("child PID = %d\n", pid1);
    }

    else{
        pid1 = getpid();
        printf("parent PID = %d\n", pid1);
        wait(NULL);
    }

    return 0;
}