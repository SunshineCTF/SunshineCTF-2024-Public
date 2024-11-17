#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FLAG_SIZE 55
#define MAX_USERNAME_SIZE 16

// just for aesthetics:
#define RESET_COLOR "\e[m"
#define GREEN "\e[32m"
#define MAGENTA "\e[35m"
#define YELLOW "\e[33m"
#define RED "\e[31m"


void clear_buffer(void) {
    int c = 0;
    while (c != '\n' && c != EOF) {
        c = getchar();
    }
}

void clear(void) {
    printf("\e[1;1H\e[2J");
}

__attribute__((noreturn))
static inline void exit_bad_input(void) {
    printf(RED "[ Invalid input! ]\n" RESET_COLOR);
    exit(1);
}

/* 
    Simply reads flag and stores string in heap.
*/
char* read_flag(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf(RED "[ Please have the flag.txt file in the current directory! ]\n");
        exit(1);
    }

    char *flag = malloc(MAX_FLAG_SIZE * sizeof(char));
    if (flag == NULL) {
        printf(RED "[ Unable to allocate memory! ]\n");
        fclose(file);
        exit(1);
    }

    if (fgets(flag, MAX_FLAG_SIZE, file) != NULL) {
        size_t len = strlen(flag);
        if (len > 0 && flag[len - 1] == '\n') {
            flag[len - 1] = '\0';
        }
    } else {
        printf(RED "[ Failed to read the file! ]\n");
        free(flag);
        exit(1);
    }

    fclose(file);
    return flag;
}


int is_dangerous_input(char* user_input) {
    // can't risk any string formatting shenanigans!
    char* black_listed = "~`!@#$%^&*()_+-={}|[]\\:\";'<>?,./";

    for (int i = 0; user_input[i] != '\0'; i++) {
        if (strchr(black_listed, user_input[i])) {
            return 1;
        }
    }

    return 0;
}


typedef struct user {
    char pronouns[8];
    char username[MAX_USERNAME_SIZE];
    int is_admin;
} user;

void create_account(user* account) {
    clear();
    printf(GREEN
            "==========================================\n\n"
            "[ Welcome to " MAGENTA "alimuhammadsecured's "  YELLOW "SECURE shop! ]\n\n"
            "[ I started a new business... dropshipping flags! ]\n"
            "[ Create an account to continue: ]\n"
            "\n"
            GREEN "==========================================\n" RESET_COLOR
    );

    printf(YELLOW "\n[ Enter your username ]\n" RESET_COLOR);
    if (scanf("%15s", account->username) != 1) {
        exit_bad_input();
    }

    if (is_dangerous_input(account->username) == 1) {
        printf(RED "[ Your userinput is dangerous! ]\n");
        exit(1);
    }

    printf(YELLOW "[ Enter your pronouns ]\n" RESET_COLOR);
    clear_buffer();
    if (scanf("%7s", account->pronouns) != 1) {
        exit_bad_input();
    }
    clear();
    account->is_admin = 0;
}

void load_panel(user* account) {
    clear();

    if (account->is_admin == 0) {
        printf(RED "\n[ Access Denied! ]\n" RESET_COLOR);
        return;
    }

    char* flag = read_flag("flag.txt");
    printf(GREEN "[ SUCCESS! Here's your flag current user: ");
    printf(account->username);
    free(flag);
    exit(0);
}

void print_user(user *account) {
    clear();

    printf(GREEN "==========================================\n");
    printf(YELLOW "Username: %s\n", account->username);
    printf(YELLOW "Pronouns: %s\n", account->pronouns);
    printf(YELLOW "is_admin: %d\n", account->is_admin);
    printf(GREEN "==========================================\n" RESET_COLOR);

    printf(YELLOW "[ ENTER to continue. ]\n" RESET_COLOR);
    clear_buffer();
    getchar();
    clear();
}

int main(void) {
    clear();

    user account = {0};
    create_account(&account);

    while (1) {
        char choice[2];
        printf(
                GREEN "==========================================\n\n"
                YELLOW "[ 1) Access Admin Panel ]\n"
                YELLOW "[ 2) Print User Information ]\n"
                "\n"
                GREEN "==========================================\n" RESET_COLOR
        );

        if (scanf("%s", &choice) != 1) {
            exit_bad_input();
        }

        switch (choice[0]) {
            case '1':
                load_panel(&account);
                break;
            case '2':
                print_user(&account);
                break;
            default:
                printf(RED "[ Invalid Option. ]\n" RESET_COLOR);
                break;
        }
    }

    return 0;
}
