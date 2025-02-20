#include <stdio.h>
#include <pthread.h>

int shared_var = 0;       

// +100
void* increment(void* arg) {
    for (int i = 0; i < 100000; i++) {
        shared_var += 100;     
    }
    return NULL;
}

// -100
void* decrement(void* arg) {
    for (int i = 0; i < 100000; i++) {
        shared_var -= 100;     
    }
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, increment, NULL);
    if (thread1)
        printf("thread1 create success!\n");
    else return 1;

    pthread_create(&thread2, NULL, decrement, NULL);
    if (thread2)
        printf("thread2 create success!\n");
    else return 1;

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("variable result: %d\n", shared_var);

    return 0;
}
