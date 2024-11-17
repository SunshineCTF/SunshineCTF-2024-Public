#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


#define MAX_POCKETS 6
#define POCKET_SIZE 64

__attribute__((constructor)) void ignore_me() {
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
}


char *knapsack[MAX_POCKETS+1] = {NULL};
int used[MAX_POCKETS+1] = {0};

void openPocket(int index);
void placeItem(int index, const char *item);
void useItem(int index, char *event);
void removeItem(int index);
void encounterGenie();
void jungleAdventure();
void handleEvent(char *event);

void openPocket(int index) {
    if (knapsack[index] == NULL) {
        knapsack[index] = (char *)malloc(POCKET_SIZE);
        used[index]=1;
        if (knapsack[index] == NULL) {
            printf("<<< Failed to allocate memory for pocket %d!\n", index);
            exit(1);
        }
        printf("<<< Opened pocket %d.\n", index);
    } 
}

void placeItem(int index, const char *item) {
    if (knapsack[index] == NULL) {
        printf("<<< Pocket %d is not open.\n", index);
        } else {
        if (used[index]==0) {
           knapsack[index]=malloc(POCKET_SIZE);
        }
        strncpy(knapsack[index], item, POCKET_SIZE - 1);
        knapsack[index][POCKET_SIZE - 1] = '\0';
        printf("<<< Placed %s in pocket %d.\n", item, index);
    }
}

void useItem(int index, char *event) {
    if (used[index]==1) {
        printf("<<< Using item from pocket %d: %s\n", index, knapsack[index]);
        if (strcmp(knapsack[index], "Machete") == 0 && strcmp(event, "tiger") == 0) {
            printf("<<< You fend off the tiger with your Machete!\n");
        } else if (strcmp(knapsack[index], "Medical Kit") == 0 && strcmp(event, "malaria") == 0) {
            printf("<<< You treat your malaria with the Medical Kit and recover!\n");
        } else if (strcmp(knapsack[index], "Compass") == 0 && strcmp(event, "lost") == 0) {
            printf("<<< You find your way back with the Compass!\n");
        } 
        else if ((strncmp(knapsack[index], "Genie",5) == 0)) {
            printf("<<< The genie unrolls a shimmering map showing a secret starting point: %p\n", &printf);
        } 
        else {
            printf("<<< The %s isn't effective against the %s.\n", knapsack[index], event);
        }
    }
    else {
        printf("<<< Pocket %d is empty.",index);
    }
}

void removeItem(int index) {
    if (knapsack[index] != NULL && used[index]==1) {    
        free(knapsack[index]);
        printf("<<< Removed item from pocket %d.\n", index );
    } else {
        printf("<<< Pocket %d is already empty.\n", index );
    }
    used[index]=!used[index];
}


void handleEvent(char *event) {
    if (strcmp(event, "tiger") == 0) {
        printf("<<< A wild tiger blocks your path!\n");
    } else if (strcmp(event, "malaria") == 0) {
        printf("<<< You feel feverish and weak... it must be malaria!\n");
    } else if (strcmp(event, "lost") == 0) {
        printf("<<< You've lost your way in the dense jungle.\n");
    } else if (strcmp(event, "crocodiles") == 0) {
        printf("<<< You encounter a river full of crocodiles blocking your way!\n");
    } else if (strcmp(event, "ancient ruin") == 0) {
        printf("<<< You discover an ancient ruin! Perhaps something special can reveal its secrets.\n");
    }
}

void __attribute__((stack_protect)) jungleAdventure() {
    int choice, pocket;
    char *items[] = {"Machete", "Medical Kit", "Compass", "Water Bottle", "Food Rations", "Lighter"};
    char *events[] = {"tiger", "malaria", "lost", "crocodiles", "ancient ruin", "nothing"};

    srand(time(0));
    printf("Welcome to the Jungle Adventure!\n");

    for (int i = 1; i < 7; i++) {
        openPocket(i);
        placeItem(i, items[i-1]);
    }

    for (int turn = 1; turn <= 25; turn++) {
        printf("\n=== Turn %d ===\n", turn);
        int eventIndex = rand() % 5;
        handleEvent(events[eventIndex]);

        printf("\n=== Choose an action ===\n");
        printf("1. Use supplies\n2. Add supplies\n3. Remove supplies\n");
        printf("Enter your choice >>> ");
        scanf("%d", &choice);
        getchar();  

        switch (choice) {
            case 1:
                printf("Use item from which pocket (1-6) >>> ");
                scanf("%d", &pocket);
                getchar();
                if (pocket >= 1 && pocket <= 6) useItem(pocket, events[eventIndex]);
                else printf("<<< Invalid pocket number.\n");
                break;
            case 2:
                printf("Select a pocket to place an item in (1-6) >>> ");
                scanf("%d", &pocket);
                getchar();
                if (pocket >= 1 && pocket <= 6) {
                    printf("Enter the item name >>> ");
        

                    if (used[pocket]==0) {
                        knapsack[pocket]=malloc(POCKET_SIZE);
                        used[pocket]=1;
                    }

                    char *ptr = knapsack[pocket];
                    memset(ptr, 0, 24);
                    read(0,ptr,POCKET_SIZE);

                } else {
                    printf("<<< Invalid pocket number.\n");
                }
                break;
            case 3:
                printf("Select a pocket to remove an item from (1-6) >>> ");
                scanf("%d", &pocket);
                getchar();
                if (pocket >= 1 && pocket <= 6) removeItem(pocket);
                else printf("<<< Invalid pocket number.\n");
                break;
            default:
                printf("<<< Invalid choice.\n");
        }
    }

   
}

int main() {
    jungleAdventure();
    return 0;
}
