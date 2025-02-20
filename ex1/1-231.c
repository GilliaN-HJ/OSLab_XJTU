#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <stdlib.h>

void* pthread_1(void* arg) {
    printf("thread1 tid = %d, pid = %ld\n", syscall(SYS_gettid), getpid());
    system("./system_call");
    printf("thread1 systemcall return\n");
    return NULL;
}

void* pthread_2(void* arg) {
    printf("thread2 tid = %d, pid = %ld\n", syscall(SYS_gettid), getpid());
    system("./system_call");
    printf("thread2 systemcall return\n");
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, pthread_1, NULL);
    if (thread1)
        printf("thread1 create success!\n");
    else return 1;

    pthread_create(&thread2, NULL, pthread_2, NULL);
    if (thread2)
        printf("thread2 create success!\n");
    else return 1;

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}