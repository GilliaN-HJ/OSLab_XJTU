#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>


int handled = 0;
//int flag = 0; 
pid_t pid1 = -1, pid2 = -1;
int terminated_1 = 0, terminated_2 = 0;

void inter_handler(int signum) {
    // TODO: 处理不同信号
    if (handled) return; // 保证只处理一次信号
    handled = 1;

    printf("\n%d stop test\n", signum); // SIGINT 对应 Ctrl + C, SIGQUIT 对应 Ctrl + '\'

    if (signum == SIGQUIT || signum == SIGINT || signum == SIGALRM){
        kill(pid1, SIGSTKFLT); // 发送信号 16
        kill(pid2, SIGCHLD); // 发送信号 17
        // kill(pid1, SIGALRM); // 闹钟中断 14
        // kill(pid2, SIGALRM); // 闹钟中断 14
    }
}

void handle_usr1(int signum) {
    terminated_1 = 1;
    printf("\n%d stop test\n", signum); // SIGSTKFLT
}

void handle_usr2(int signum) {
    terminated_2 = 1;
    printf("\n%d stop test\n", signum); // SIGCHLD
}

void block_sig() { 
    // 在子进程中屏蔽SIGINT和SIGQUIT信号
    sigset_t set; 
    sigemptyset(&set); 
    sigaddset(&set, SIGINT); 
    sigaddset(&set, SIGQUIT);
    sigprocmask(SIG_BLOCK, &set, NULL); 
}

int main() {
    // TODO: 五秒之后或接收到两个信号
    while (pid1 == -1) pid1=fork();
    if (pid1 > 0) {
        while (pid2 == -1) pid2=fork();
        if (pid2 > 0) {
            // TODO: 父进程
                    // 信号处理函数
            signal(SIGINT, inter_handler);  // 捕捉 SIGINT（如 Ctrl + C）
            signal(SIGQUIT, inter_handler); // 捕捉 SIGQUIT（如 Ctrl + \）
            signal(SIGALRM, inter_handler);  // 捕捉 SIGALRM（超时信号）
            alarm(5); 
            pause();

            wait(NULL);
            wait(NULL); 
            printf("\nParent process is killed!!\n");

            return 0;
        } 
        else {
            // TODO: 子进程 2
            block_sig();
            signal(SIGCHLD, handle_usr2);
            // signal(SIGALRM, handle_usr2);
            pause();
            if (terminated_2){
                printf("\nChild process2 is killed by parent!!\n");
                exit(0);
            }
        }
    } 
    else {
        // TODO：子进程 1
        block_sig();
        signal(SIGSTKFLT, handle_usr1);
        // signal(SIGALRM, handle_usr1);
        pause();
        if (terminated_1){
            printf("\nChild process1 is killed by parent!!\n");
            exit(0);
        }  
 }
 return 0;
}
