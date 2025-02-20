/*
1 - Set memory size (default=1024)
2 - Select memory allocation algorithm
3 - New process
4 - Terminate a process
5 - Display memory usage
0 - Exit
*/
#include <stdio.h>
#include <stdlib.h>

#define PROCESS_NAME_LEN 32 /*进程名长度*/
#define MIN_SLICE 10 /*最小碎片的大小*/
#define DEFAULT_MEM_SIZE 1024 /*内存大小*/
#define DEFAULT_MEM_START 0 /*起始位置*/
/* 内存分配算法 */
#define MA_FF 1
#define MA_BF 2
#define MA_WF 3
int mem_size=DEFAULT_MEM_SIZE; /*内存大小*/
int ma_algorithm = MA_FF; /*当前分配算法*/
static int pid = 0; /*初始 pid*/
int flag = 0; /*设置内存大小标志*/

/*描述每一个空闲块的数据结构*/
struct free_block_type{
    int size;
    int start_addr;
    struct free_block_type *next;
};
/*指向内存中空闲块链表的首指针*/
struct free_block_type *free_block;

/*每个进程分配到的内存块的描述*/
struct allocated_block{
    int pid; int size;
    int start_addr;
    char process_name[PROCESS_NAME_LEN];
    struct allocated_block *next;
};
/*进程分配内存块链表的首指针*/
struct allocated_block *allocated_block_head = NULL;

/*初始化空闲块，默认为一块，可以指定大小及起始地址*/
struct free_block_type* init_free_block(int mem_size){
    struct free_block_type *fb;
    fb=(struct free_block_type *)malloc(sizeof(struct free_block_type));
    if(fb==NULL){
        printf("No mem\n");
        return NULL;
    }
    fb->size = mem_size;
    fb->start_addr = DEFAULT_MEM_START;
    fb->next = NULL;
    return fb;
}

/*设置内存的大小*/
int set_mem_size(){
    int size;
    if(flag!=0){ //防止重复设置
        printf("Cannot set memory size again\n");
        return 0;
    }
    printf("Total memory size =");
    scanf("%d", &size);
    if(size>0) {
        mem_size = size;
        // init_free_block(mem_size);
        free_block->size = mem_size;
    }
    flag=1; 
    return 1;
}

/*按 FF 算法重新整理内存空闲块链表*/
// 
void rearrange_FF() {
    struct free_block_type *i, *j;
    int temp_size, temp_start;

    // 按起始地址升序排列空闲块链表
    for (i = free_block; i != NULL; i = i->next) {
        for (j = i->next; j != NULL; j = j->next) {
            if (i->start_addr > j->start_addr) {
                // 交换 i 和 j 的大小和起始地址
                temp_size = i->size;
                temp_start = i->start_addr;

                i->size = j->size;
                i->start_addr = j->start_addr;

                j->size = temp_size;
                j->start_addr = temp_start;
            }
        }
    }
}

/*按 BF 算法重新整理内存空闲块链表*/
void rearrange_BF(){
    struct free_block_type *i, *j;
    int temp_size, temp_start;
    // 按照空闲块大小升序排列
    for(i = free_block; i != NULL; i = i->next){
        for(j = i->next; j != NULL; j = j->next){
            if(i->size > j->size){
                temp_size = i->size;
                temp_start = i->start_addr;
                i->size = j->size;
                i->start_addr = j->start_addr;
                j->size = temp_size;
                j->start_addr = temp_start;
            }
        }
    }
}

/*按 WF 算法重新整理内存空闲块链表*/
void rearrange_WF(){
    struct free_block_type *i, *j;
    int temp_size, temp_start;
    // 按照空闲块大小降序排列
    for(i = free_block; i != NULL; i = i->next){
        for(j = i->next; j != NULL; j = j->next){
            if(i->size < j->size){
                temp_size = i->size;
                temp_start = i->start_addr;
                i->size = j->size;
                i->start_addr = j->start_addr;
                j->size = temp_size;
                j->start_addr = temp_start;
            }
        }
    }
}

/*按指定的算法整理内存空闲块链表*/
void rearrange(int algorithm){
    switch(algorithm){
        case MA_FF: rearrange_FF(); break;
        case MA_BF: rearrange_BF(); break;
        case MA_WF: rearrange_WF(); break;
    }
}

/* 设置当前的分配算法 */
void set_algorithm(){
    int algorithm;
    printf("\t1 - First Fit\n");
    printf("\t2 - Best Fit \n");
    printf("\t3 - Worst Fit \n");
    scanf("%d", &algorithm);
    if(algorithm>=1 && algorithm <=3)
    ma_algorithm=algorithm;
    //按指定算法重新排列空闲区链表
    rearrange(ma_algorithm);
}

/* 内存紧缩函数 */
void compact_free_blocks(int free_block_size) {
    struct free_block_type *cur = free_block;
    struct free_block_type *new_block;
    struct allocated_block *ab = allocated_block_head;
    int current_address = 0;

    // 创建一个新的空闲块表示紧缩后的连续区域
    new_block = (struct free_block_type *)malloc(sizeof(struct free_block_type));
    if (!new_block) {
        printf("Memory compaction failed: no sufficient system memory.\n");
        return;
    }
    new_block->start_addr = DEFAULT_MEM_START; // 紧缩后从内存起始地址开始
    new_block->size = free_block_size;
    new_block->next = NULL;

    // 释放原有空闲块链表
    cur = free_block;
    while (cur != NULL) {
        struct free_block_type *temp = cur;
        cur = cur->next;
        free(temp);
    }

    // 更新空闲块链表为紧缩后的新块
    free_block = new_block;

    printf("Memory compaction completed: total free size = %d.\n", free_block_size);

    // 紧缩已分配块
    current_address = free_block->start_addr + free_block->size;
    while (ab != NULL) {
        ab->start_addr = current_address;
        current_address += ab->size;
        ab = ab->next;
    }

}

/*分配内存模块*/
int allocate_mem(struct allocated_block *ab){
    struct free_block_type *fbt, *pre, *tfbt; //pre:前一个 tfbt = fbt
    int request_size=ab->size;
    int free_block_size = 0;
    fbt = free_block; 
    pre = NULL;
    tfbt = fbt;
    //计算所有剩余空闲块的大小
    while (tfbt != NULL){
        free_block_size += tfbt->size;
        tfbt = tfbt->next;
    }
    //在空闲分区链表中搜索合适空闲分区进行分配
    while (fbt != NULL){
        //找到可满足空闲分区且分配后剩余空间足够大，则分割
        if (fbt->size - request_size >= MIN_SLICE){
            ab->start_addr = fbt->start_addr;
            fbt->start_addr += request_size;
            fbt->size -= request_size;
            return 1;
        }
        //找到可满足空闲分区且但分配后剩余空间比较小，则一起分配
        else if (fbt->size >= request_size && fbt->size - request_size < MIN_SLICE){
            ab->start_addr = fbt->start_addr;
            if(pre == NULL) free_block = fbt->next;
            else pre->next = fbt->next;
            free(fbt);
            return 1;
        }
        //找不可满足需要的空闲分区但空闲分区之和能满足需要，则采用内存紧缩技术，进行空闲分区的合并，然后再分配
        else if (fbt->size < request_size && free_block_size >= request_size){
            compact_free_blocks(free_block_size);
            return allocate_mem(ab);
        }
        //分配不成功
        else {
            pre = fbt;
            fbt = fbt->next;
        }
    }
    return -1;
}

/*创建新的进程，主要是获取内存的申请数量*/
int new_process(){
    struct allocated_block *ab;
    int size; int ret;
    ab=(struct allocated_block *)malloc(sizeof(struct allocated_block));
    if(!ab) exit(-5);
    ab->next = NULL;
    pid++;
    sprintf(ab->process_name, "PROCESS-%02d", pid);
    ab->pid = pid;
    printf("Memory for %s:", ab->process_name);
    scanf("%d", &size);
    if(size>0) ab->size=size;
    ret = allocate_mem(ab); /* 从空闲区分配内存，ret==1 表示分配 ok*/
    /*如果此时 allocated_block_head 尚未赋值，则赋值*/
    if((ret==1) &&(allocated_block_head == NULL)){
        allocated_block_head=ab;
        return 1; }
    /*分配成功，将该已分配块的描述插入已分配链表*/
    else if (ret==1) {
        ab->next=allocated_block_head;
        allocated_block_head=ab;
        return 2; }
    else if(ret==-1){ /*分配不成功*/
        printf("Allocation fail\n");
        free(ab);
        return -1;
    }
    return 3;
}

/*将 ab 所表示的已分配区归还，并进行可能的合并*/
int free_mem(struct allocated_block *ab){
    struct free_block_type *fbt, *pre = NULL;
    struct free_block_type *cur = free_block;
    fbt=(struct free_block_type*) malloc(sizeof(struct free_block_type));
    if(!fbt) return -1;
    fbt->size = ab->size;
    fbt->start_addr = ab->start_addr;
    fbt->next = NULL;

    // 插入空闲块列表并按地址排序
    rearrange_FF();
        // 找到合适的位置
    while(cur != NULL && cur->start_addr < fbt->start_addr){
        pre = cur;
        cur = cur->next;
    }
        // 插入
    if(pre == NULL){
        fbt->next = free_block;
        free_block = fbt;
    } else {
        fbt->next = pre->next;
        pre->next = fbt;
    }

    // 合并相邻空闲块
        cur = free_block;
        while(cur != NULL && cur->next != NULL){
            if(cur->start_addr + cur->size == cur->next->start_addr){
                cur->size += cur->next->size;
                struct free_block_type *temp = cur->next;
                cur->next = cur->next->next;
                free(temp); 
            } else {
                cur = cur->next;
            }
        }
    rearrange(ma_algorithm); // 按当前算法重新排列空闲链表
    return 1;
}

/*释放 ab 数据结构节点*/
int dispose(struct allocated_block *free_ab){
    struct allocated_block *pre, *ab;
    if(free_ab == allocated_block_head) { /*如果要释放第一个节点*/
        allocated_block_head = allocated_block_head->next;
        free(free_ab);
        return 1;
    }
    pre = allocated_block_head;
    ab = allocated_block_head->next;
    while(ab!=free_ab){ 
        pre = ab; 
        ab = ab->next; 
    }
    pre->next = ab->next;
    free(ab);
    return 2;
}

/* 显示当前内存的使用情况，包括空闲区的情况和已经分配的情况 */
int display_mem_usage(){
    struct free_block_type *fbt=free_block;
    struct allocated_block *ab=allocated_block_head;
    // if(fbt == NULL && ab == NULL) {
    if(fbt == NULL) {
        printf("No free memory.\n");
        // return(-1);
    }
    printf("----------------------------------------------------------\n");
    /* 显示空闲区 */
    printf("Free Memory:\n");
    printf("%20s %20s\n", " start_addr", " size");
    while(fbt!=NULL){
    printf("%20d %20d\n", fbt->start_addr, fbt->size);
    fbt=fbt->next;
    }
    /* 显示已分配区 */
    printf("\nUsed Memory:\n");
    printf("%10s %20s %10s %10s\n", "PID", "ProcessName", "start_addr", " size");
    while(ab!=NULL){
        printf("%10d %20s %10d %10d\n", ab->pid, ab->process_name,
        ab->start_addr, ab->size);
        ab=ab->next;
    }
    printf("----------------------------------------------------------\n");
    return 0;
}

/*找到pid对应的存储空间*/
struct allocated_block* find_process(int pid) {
    struct allocated_block *ab = allocated_block_head;
    while (ab != NULL) {
        if (ab->pid == pid) {
            return ab;
        }
        ab = ab->next;
    }
    return NULL; // 找不到该pid
}

/*删除进程，归还分配的存储空间，并删除描述该进程内存分配的节点*/
void kill_process(){
    struct allocated_block *ab;
    int pid;
    printf("Kill Process, pid=");
    scanf("%d", &pid);
    ab=find_process(pid);
    if(ab!=NULL){
        free_mem(ab); /*释放 ab 所表示的分配区*/
        dispose(ab); /*释放 ab 数据结构节点*/
    }
    else printf("This PID is invalid.\n");
}

void do_exit() {
    struct free_block_type *free_ptr;
    struct allocated_block *alloc_ptr;

    // 释放空闲块链表
    while (free_block != NULL) {
        free_ptr = free_block;
        free_block = free_block->next;
        free(free_ptr);
    }

    // 释放已分配块链表
    while (allocated_block_head != NULL) {
        alloc_ptr = allocated_block_head;
        allocated_block_head = allocated_block_head->next;
        free(alloc_ptr);
    }

    printf("Memory successfully released. Exiting program.\n");
}

/*显示菜单*/
void display_menu(){
    printf("\n");
    printf("1 - Set memory size (default=%d)\n", DEFAULT_MEM_SIZE);
    printf("2 - Select memory allocation algorithm\n");
    printf("3 - New process \n");
    printf("4 - Terminate a process \n");
    printf("5 - Display memory usage \n");
    printf("0 - Exit\n");
}

int main(){
    char choice; pid=0;
    free_block = init_free_block(mem_size); //初始化空闲区
    while(1) {
        if (choice != '\n') // 清除缓冲区的'\n'
            display_menu(); //显示菜单
        fflush(stdin);
        choice=getchar(); //获取用户输入
        switch(choice){
            case '1': set_mem_size(); break; //设置内存大小
            case '2': set_algorithm();flag=1; break;//设置算法
            case '3': new_process(); flag=1; break;//创建新进程
            case '4': kill_process(); flag=1; break;//删除进程
            case '5': display_mem_usage(); flag=1; break; //显示内存使用
            case '0': do_exit(); exit(0); //释放链表并退出
            default: break; 
        } 
    }
}
