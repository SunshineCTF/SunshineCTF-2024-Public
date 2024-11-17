/*
    Author: Oreomeister
    Twitter & Github: 0reome1ster
*/

#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/seccomp.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <linux/audit.h>
#include <linux/bpf.h>
#include <linux/filter.h>
#include <linux/unistd.h>
#include <stddef.h>
#include <sys/prctl.h>
#include <string.h>

void win();

#define MAX_FLAGS 4

char *flags[MAX_FLAGS];
int num_flags = 0;

unsigned char *logo = \
"   _____                            ______ _            \n" 
"  / ____|                          |  ____| |            \n"
" | (___   ___  ___ _   _ _ __ ___  | |__  | | __ _  __ _ \n"
"  \\___ \\ / _ \\/ __| | | | '__/ _ \\ |  __| | |/ _` |/ _` |\n"
"  ____) |  __/ (__| |_| | | |  __/ | |    | | (_| | (_| |\n"
" |_____/_\\___|\\___|\\__,_|_|  \\___| |_|    |_|\\__,_|\\__, |\n"
"       |__   __|                (_)           | |   __/ |\n"
"          | | ___ _ __ _ __ ___  _ _ __   __ _| |  |___/ \n"
"          | |/ _ \\ '__| '_ ` _ \\| | '_ \\ / _` | |        \n"
"          | |  __/ |  | | | | | | | | | | (_| | |        \n"
"          |_|\\___|_|  |_| |_| |_|_|_| |_|\\__,_|_|        \n"
"                                                         \n"
"                                                         ";
#define INVALID(x) ({ \
    __typeof__(x) _x = (x); \
    _x < 0 || _x >= num_flags || flags[_x] == NULL; \
})

// Securely generates seed
unsigned int generate_seed() {
    unsigned int seed = time(NULL);
    seed ^= getpid();
    seed ^= (unsigned int) &seed;
    return seed;
}

// For secure random number generation
__attribute__((constructor)) void ignore_me() {
    srand(generate_seed());
}

// Function to apply seccomp filters
static int apply_filters() {
    struct sock_filter filter[] = {
        BPF_STMT(BPF_LD+BPF_W+BPF_ABS, (offsetof(struct seccomp_data, arch))), 
        BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, AUDIT_ARCH_X86_64, 1, 0), 
        BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_ERRNO),
        BPF_STMT(BPF_LD+BPF_W+BPF_ABS, (offsetof(struct seccomp_data, nr))),
        BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, __NR_write, 0, 1),
        BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_ALLOW),
        BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, __NR_read, 0, 1),
        BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_ALLOW),
        BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, __NR_brk, 0, 1),
        BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_ALLOW),
        BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, __NR_close, 0, 1),
        BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_ALLOW),
        BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, __NR_fstat, 0, 1),
        BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_ALLOW),
        BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, __NR_lseek, 0, 1),
        BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_ALLOW),
        BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, __NR_exit_group, 0, 1),
        BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_ALLOW),
        BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_KILL)
    };

    struct sock_fprog prog = {
		.len = (unsigned short)(sizeof(filter)/sizeof(filter[0])),
		.filter = filter,
	};

	if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0)) {
        return 1;
    }

	if (prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &prog)) {
        return 1;
    }

    return 0;
}

// Get handle to fd to flag.txt
int *open_flag() {
    int *buf = (int *)malloc(sizeof(int));
    int tmp = open("./flag.txt", O_RDONLY);

    int new_fd = (rand() % 1029) + 3;

    dup2(tmp, new_fd);

    close(tmp);

    *buf = new_fd;

    return buf;
}

void win(ulong value)
{
    if (value == (ulong)win)
        system("/bin/sh");
    
    printf("Uh oh, I guess you didn't meet the win condition :(\n");
}

// Gets seed for terminal
ulong get_seed(void) {
    int err = ptrace(PTRACE_TRACEME, getpid(), NULL, NULL);

    if (err < 0)
       return (ulong)rand ^ 0xd3c0dead;

    return (ulong)rand;
}

// Creates flag ptr
void touch_flag()
{
    ulong chunk_size = 0;

    if (num_flags >= MAX_FLAGS) {
        printf("Uh oh, storage array is out of space...\n");
        return;
    }

    printf("Enter size of flag --> ");
    scanf("%lu", &chunk_size);

    printf("Allocating space within storage array...\n");
    flags[num_flags] = (char *)malloc(chunk_size);
    
    if (flags[num_flags] == NULL) {
        printf("ERROR: Terminal has ran out of RAM :(\n\n");
        return;
    }

    num_flags += 1;
    printf("SUCCESS: Flag #%d can be modified now!\n\n", num_flags);
}

// Edits the flag
void vim_flag()
{
    int ind;
    printf("Enter flag # to edit --> ");
    scanf("%d", &ind);
    ind -= 1;

    if (INVALID(ind)) {
        printf("Invalid flag # >:(\n\n");
        return;
    }

    printf("Enter new flag --> ");
    read(0, flags[ind], 0xb4);
    printf("SUCCESS: Edited flag #%d... quitting vim...\n\n", ind + 1);
}

// Prints flag
void cat_flag()
{
    int ind;
    printf("Enter flag # to view --> ");
    scanf("%d", &ind);
    ind -= 1;

    if (INVALID(ind)) {
        printf("Invalid flag # >:(\n\n");
        return;
    }

    printf("===== Flag #%d =====\n", ind);
    printf("\n%s\n", flags[ind]);
    printf("====================\n\n");

}

// "Defragments" the storage
void shift_flags(int ind)
{
    printf("Defragmenting storage array...\n");
    for (int i = ind; i < (num_flags - 1); i++) {
        flags[i] = flags[i + 1];
    }
}

// Deletes flag
void rm_flag()
{
    int ind;
    printf("Enter flag # to remove --> ");
    scanf("%d", &ind);
    ind -= 1;

    if (INVALID(ind)) {
        printf("Invalid flag # >:(\n\n");
        return;
    }

    printf("Removing flag #%d...\n", ind + 1);
    free(flags[ind]);
    shift_flags(ind);
    num_flags -= 1;
    printf("SUCCESS: Flag removed!\n\n");
}

void print_logo()
{
    printf("%s\n", logo);
    printf("\t\tKernel Seed: %p\n\n",(void *)get_seed());
    
}

int menu()
{
    int ret;

    printf("++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");
    printf("\t1. $ touch flag\n");
    printf("\t2. $ vim flag\n");
    printf("\t3. $ cat flag\n");
    printf("\t4. $ rm flag\n");
    printf("\nRemaining storage: %d\n", MAX_FLAGS - num_flags);
    printf("++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");

    printf("Enter option: ");
    scanf("%d", &ret);
    printf("\n");

    return ret;
}

int main(void)
{   
    int *flag;

    print_logo();
    int choice = menu();
    flag = open_flag();
    apply_filters();


    while (choice)
    {
        switch(choice)
        {
            case 1:
                touch_flag();
                break;
            case 2:
                vim_flag();
                break;
            case 3:
                cat_flag();
                break;
            case 4:
                rm_flag();
                break;
            default:
                printf("Invalid choice... do better\n");
                goto quit;
        }
        
        choice = menu();
    }

quit:
    close(*flag);
    free(flag);

    return 0;
}
