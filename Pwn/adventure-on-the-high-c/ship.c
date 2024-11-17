#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define BOARD_SIZE 16
#define NUM_SHIPS 5
#define TRUE 1
#define FALSE 0

int DEBUG = FALSE;

__attribute__((constructor)) void ignore_me() {
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
}

int logo() {
    printf("\033[1;32m================================================================================\n\033[0m");
    printf("\nMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMcat<<<$(<flag.txt)\0\n");
    printf("\nMMMMMMMMMMMMMMMMMMWWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMcat<flag.txt\0\n");
    printf("\nMMMMMMMMMMMMMMWNXNk;dNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMprintf%%q$(<flag.txt)\0\n");
    printf("\nMMMMMMMMMN0xoc,.;Od.:NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n");
    printf("MMMMMMMMMNOo:'. .Od.:NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n");
    printf("MMMMMMMMMMMMMNKkkXo.:NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n");
    printf("MMMMMMMMMMMMMMMMMWo.:NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n");
    printf("MMMMMMMMMMMMMMMMMWo.:NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n");
    printf("MMMMMMMMMMMMMMMMMWo.:NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n");
    printf("MMMMMMMMMMMMMMMMMWo.:NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n");
    printf("MMMMMMMMMMMMMMMMMWo.:NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n");
    printf("MMMMMMMMMMMMMMMMMWo.:NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n");
    printf("MMMMMMMMMMMMMMMMMWo :NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWNX0Okxkxdd0WM\n");
    printf("MMMMMMMMMMMMMMMMMWx'lNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWMMMMWKdc:,'..         M\n");
    printf("MMMMMMMMMMMWXOOOOkxodkOOOOKWMMMMMMMMMMMMMMMMMMMMMMNOolcdK0occlxd'       .,cod0WM\n");
    printf("MMMMMMMMMMXo.             .cKMMMMMMMMMMMMMMMMMMMMMK,  .xk.    .xOlcloxk0KNMMMMMM\n");
    printf("MMMMMMMMMMO.   \033[0;35mFlagShip\033[0m    .dWMMMMMMMMMMMMMMMMMMMMWd''lKo      cNMMMMMMMMMMMMMMM\n");
    printf("MMMMMMMMMMk.                oWMMMMMMMMMMMMMMMMMMMMMWNNWWOc:::::kWMMMMMMMMMMMMMMM\n");
    printf("MMMMMMMMMMKl;;;;;;;;;;;;;;;:OWMMMMMMMMMMMMMMMWKoccccccccccccccccccccccccccccccco\n");
    printf("MMMMMMWKxllcclcccccccccccclclld0WMMMMMMMMMMWKl.                .clc'  .:ll,    :\n");
    printf("MMMMMWx.                       .oNMMMMMMMMXo.                 .kOlOk..d0lx0,  ,0\n");
    printf("MMMMMNd''''''',,,,,,,,,,;;;;;;;;oXMMMMMX0d'                    :dddc. ;ddxl. .kW\n");
    printf("xdddddolllllllllllllllllllllllllllllllc'.                        .      .   .xWM\n");
    printf(",                                                                           lNMM\n");
    printf("0'                                                                         :XMMM\n");
    printf("Wk.                                                                       ,0MMMM\n");
    printf("MWx.                                                \033[0;35mUSS v10l3nt\033[0m          .OMMMMM\n");
    printf("MMNo.                                                                   .xWMMMMM\n");
    printf("MMMNl                                                                  .oNMMMMMM\n");
    printf("MMMMNkddddddddddddddddddddddddddddddddddddddddddddddddddddddddddxxddxddkNMMMMMMM\n");
    printf("\033[1;32mMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMsleep 60\0\n");
    printf("\nMMM               \033[0;35mAdventure! on The High C. v1337.0\033[0m                         \033[1;32mMMMM\n");
    printf("MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMcat flag.txt\0\n\033[0m");
    printf("\n================================================================================\n\033[0m");

    return 0;
}

int ship_sizes[NUM_SHIPS] = {5, 4, 3, 3, 2};

char EMPTY = '.';
char SHIP = 'S';
char HIT = 'X';

void initialize_board(char board[BOARD_SIZE][BOARD_SIZE]);
void display_board(char board[BOARD_SIZE][BOARD_SIZE], int show_ships);
void place_ships_randomly(char board[BOARD_SIZE][BOARD_SIZE]);
int fire_missile(char board[BOARD_SIZE][BOARD_SIZE], int row, int col, char missile);
int all_ships_sunk(char board[BOARD_SIZE][BOARD_SIZE]);
int is_valid_position(int row, int col);
void enemy_fire(char player_board[BOARD_SIZE][BOARD_SIZE]);

int main() {
    logo();
    char player_board[BOARD_SIZE][BOARD_SIZE];
    char enemy_board[BOARD_SIZE][BOARD_SIZE];
    char row_input, col_input;
    //unsigned 
    char missile_type;
    int row, col, game_over = 0;
    
    srand(time(NULL));

    initialize_board(player_board);
    initialize_board(enemy_board);

    place_ships_randomly(player_board);
    place_ships_randomly(enemy_board);

    while (!game_over) {
        printf("\n<<< Your Board:\n");
        display_board(player_board, TRUE); 
        
        printf("\n<<< Enemy Board:\n");
        display_board(enemy_board, DEBUG);

        printf("\nEnter the row (0-9, a-f) >>> ");
        scanf(" %i", &row);

        printf("Enter the column (0-9, a-f) >>> ");
        scanf("%i", &col);

        printf("Choose missile type - Tomahawk (T), Hellfire (H), SideWinder (S), Custom (C) >>> ");
        scanf(" %c", &missile_type);
        
      if (missile_type == 'C') {
            char custom_missile;
            printf("Enter a custom missile as a single ASCII character >>> ");
            scanf(" %c", &custom_missile);
            missile_type = custom_missile;
        }

        if (!is_valid_position(row, col)) {
            printf("<<< Fired outside board, corrupting %p from %02x to %02x!",&enemy_board[row][col],(unsigned char)enemy_board[row][col],(unsigned char)missile_type);
            enemy_board[row][col]= missile_type;
            continue;
        }

    
        if (fire_missile(enemy_board, row, col, missile_type)) {
            printf("\n<<< Direct Hit!\n");
        } else {
            printf("\n<<< Miss...\n");
        }

        if (all_ships_sunk(enemy_board)) {
            printf("\n<<< Congratulations! You have sunk all enemy ships! Victory!\n");
            game_over = 1;
            continue;
        }

        printf("\n<<< Enemy's Turn to Fire...\n");
        enemy_fire(player_board);

        if (all_ships_sunk(player_board)) {
            printf("\n<<< All your ships have been sunk. The enemy wins!\n");
            game_over = 1;
        }
    }

    return 0;
}

__attribute__((naked)) void gadget() {
    __asm__ volatile (
        "pop %rdi\n"   
        "ret\n"        
    );
    system("uname -a");
}


void initialize_board(char board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = EMPTY;
        }
    }
}

void display_board(char board[BOARD_SIZE][BOARD_SIZE], int show_ships) {
    printf("   ");
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (i < 10)
            printf(" %X ", i);
        else
            printf(" %c ", 'a' + (i - 10));
    }
    printf("\n");

    for (int i = 0; i < BOARD_SIZE; i++) {
        if (i < 10)
            printf(" %X ", i);
        else
            printf(" %c ", 'a' + (i - 10));

        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == SHIP && !show_ships) {
                printf(" %c ", EMPTY); 
            } else {
                printf(" %c ", board[i][j]);
            }
        }
        printf("\n");
    }
}

void place_ships_randomly(char board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < NUM_SHIPS; i++) {
        int ship_size = ship_sizes[i];
        int placed = 0;

        while (!placed) {
            int row = rand() % BOARD_SIZE;
            int col = rand() % BOARD_SIZE;
            int horizontal = rand() % 2; 

            int can_place = 1;
            if (horizontal) {
                if (col + ship_size >= BOARD_SIZE) continue;
                for (int j = 0; j < ship_size; j++) {
                    if (board[row][col + j] != EMPTY) {
                        can_place = 0;
                        break;
                    }
                }
            } else {
                if (row + ship_size >= BOARD_SIZE) continue;
                for (int j = 0; j < ship_size; j++) {
                    if (board[row + j][col] != EMPTY) {
                        can_place = 0;
                        break;
                    }
                }
            }

            if (can_place) {
                if (horizontal) {
                    for (int j = 0; j < ship_size; j++) {
                        board[row][col + j] = SHIP; 
                    }
                } else {
                    for (int j = 0; j < ship_size; j++) {
                        board[row + j][col] = SHIP;
                    }
                }
                placed = 1;
            }
        }
    }
}

int fire_missile(char board[BOARD_SIZE][BOARD_SIZE], int row, int col, char missile) {
    if (board[row][col] == SHIP) {
        board[row][col] = HIT;
        return 1; 
    } else if (board[row][col] == EMPTY) {
        board[row][col] = missile; 
    }
    return 0; 
}

int all_ships_sunk(char board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == SHIP) {
                return 0; 
            }
        }
    }
    return 1; 
}

int is_valid_position(int row, int col) {
    return (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE);
}

int hex_to_dec(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'a' && c <= 'f') {
        return 10 + (c - 'a');
    } else if (c >= 'A' && c <= 'F') {
        return 10 + (c - 'A');
    } else {
        return -1; 
    }
}

void enemy_fire(char player_board[BOARD_SIZE][BOARD_SIZE]) {
    int row, col;
    do {
        row = rand() % BOARD_SIZE;
        col = rand() % BOARD_SIZE;
    } while (player_board[row][col] == HIT || player_board[row][col] == 'T' || player_board[row][col] == 'H' || player_board[row][col] == 'S'); 

    printf("<<< Enemy fires at (%X, %X)\n", row, col); 

    if (player_board[row][col] == SHIP) {
        player_board[row][col] = HIT;
        printf("<<< Enemy hits one of your ships!\n");
    } else {
        player_board[row][col] = 'E'; 
        printf("<<< Enemy misses...\n");
    }
}