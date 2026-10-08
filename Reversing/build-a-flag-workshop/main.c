// Challenge by dpresso

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <openssl/md5.h>
#include <string.h>
#include <stdlib.h>

typedef struct flag_params {
    int length;
    int genre;
    int vibe;
    char signature[100];
} flag_params;

char* flags[3][3][3] = 
{
    { // Short
        { // Sci-Fi
            "live_long_prosper", // Funny: *Star Trek*
            "resistance_is_futile",  // Deep: *Star Trek*
            "hasta_la_vista_baby"   // Edgy: *Terminator 2*
        },
        { // Fantasy
            "you_shall_not_pass", // Funny: Gandalf, *The Lord of the Rings*
            "not_all_those_who_wander_are_lost",  // Deep: *The Lord of the Rings*
            "chaos_is_a_ladder"   // Edgy: *Game of Thrones*
        },
        { // IRL
            "i_feel_the_need_for_speed", // Funny: *Top Gun*
            "i_think_therefore_i_am",  // Deep: Descartes
            "what_we_do_in_life_echoes_in_eternity"   // Edgy: *Gladiator*
        }
    },
    { // Medium
        { // Sci-Fi
            "soylent_green_is_people", // Funny: *Soylent Green*
            "infinite_diversity_infinite_combinations",  // Deep: *Star Trek*
            "humanity_is_cancer"   // Edgy: Agent Smith, *The Matrix*
        },
        { // Fantasy
            "one_does_not_simply", // Funny: *The Lord of the Rings*
            "winter_is_coming",  // Deep: *Game of Thrones*
            "what_is_dead_may_never_die"   // Edgy: *Game of Thrones*
        },
        { // IRL
            "heres_looking_at_you_kid", // Funny: *Casablanca*
            "veni_vidi_vici",  // Deep: Julius Caesar
            "fear_is_the_mind_killer"   // Edgy: *Dune*
        }
    },
    { // Long
        { // Sci-Fi
            "dont_panic", // Funny: Douglas Adams, *The Hitchhiker's Guide to the Galaxy*
            "the_force_will_be_with_you_always",  // Deep: *Star Wars*
            "ive_seen_things_you_people_wouldnt_believe"   // Edgy: *Blade Runner*
        },
        { // Fantasy
            "second_breakfast", // Funny: *The Lord of the Rings*
            "all_we_have_to_decide_is_what_to_do_with_the_time_given_to_us",  // Deep: Gandalf, *The Lord of the Rings*
            "dark_and_full_of_terrors"   // Edgy: *Game of Thrones*
        },
        { // IRL
            "im_gonna_make_him_an_offer", // Funny: *The Godfather*
            "the_only_thing_we_have_to_fear_is_fear_itself",  // Deep: FDR
            "madness_is_like_gravity"   // Edgy: The Joker, *The Dark Knight*
        }
    },
};


uint8_t signature_a_hash[16] = {0xab, 0x17, 0x85, 0x09, 0x78, 0xe3, 0x6a, 0xaf, 0x6a, 0x2b, 0x88, 0x08, 0xf1, 0xde, 0xd9, 0x71};

char* gen_flag(flag_params params) {
    char* flag = calloc((strlen(flags[params.length][params.genre][params.vibe]) + strlen(params.signature) + 2), sizeof(char));
    flag = strcat(flag, flags[params.length][params.genre][params.vibe]);
    if (strlen(params.signature)) {
        strcat(flag, "-");
        strcat(flag, params.signature);
    }

    return flag;
}

void print_menu(flag_params params) {
    printf("|-----------------------------------|\n");
    printf("| Welcome to Build-A-Flag Workshop! |\n");
    printf("|-----------------------------------|\n");
    printf("| Your flag: %s\n", gen_flag(params));
    printf("|\n");
    printf("| 1: Length    [%d]\n", params.length);
    printf("| 2: Genre     [%d]\n", params.genre);
    printf("| 3: Vibe      [%d]\n", params.vibe);
    printf("| 4: Signature [%s]\n", params.signature);
    printf("|\n");
    printf("| [NA]: Chompy's flag check\n");
    printf("|\n");
    printf("| x: Exit\n");
    printf("\n");
    printf(">>> ");
}

void length_menu(flag_params* params) {
    printf("| Choose flag length |\n");
    printf("|--------------------|\n");
    printf("| 1: Short\n");
    printf("| 2: Medium\n");
    printf("| 3: Long\n");
    printf("\n");
    printf("| x: Cancel\n");
    printf(">>> ");

    char user_sel = getchar();
    while (getchar() != '\n');
    switch (user_sel)
    {
    case '1':
        params->length = 0;
        return;
    case '2':
        params->length = 1;
        return;
    case '3':
        params->length = 2;
        return;
    case 'x':
        return;
    default:
        length_menu(params);
    }
}

void genre_menu(flag_params* params) {
    printf("| Choose flag genre |\n");
    printf("|-------------------|\n");
    printf("| 1: Sci-Fi\n");
    printf("| 2: Fantasy\n");
    printf("| 3: IRL\n");
    printf("\n");
    printf("| x: Cancel\n");
    printf(">>> ");

    char user_sel = getchar();
    while (getchar() != '\n');
    switch (user_sel)
    {
    case '1':
        params->genre = 0;
        return;
    case '2':
        params->genre = 1;
        return;
    case '3':
        params->genre = 2;
        return;
    case 'x':
        return;
    default:
        genre_menu(params);
    }
}

void vibe_menu(flag_params* params) {
    printf("| Choose flag vibe |\n");
    printf("|------------------|\n");
    printf("| 1: Funny\n");
    printf("| 2: Deep\n");
    printf("| 3: Edgy\n");
    printf("\n");
    printf("| x: Cancel\n");
    printf(">>> ");

    char user_sel = getchar();
    while (getchar() != '\n');
    switch (user_sel)
    {
    case '1':
        params->vibe = 0;
        return;
    case '2':
        params->vibe = 1;
        return;
    case '3':
        params->vibe = 2;
        break;
    case 'x':
        return;
    default:
        vibe_menu(params);
    }
}

void signature_menu(flag_params* params) {
    printf("| Enter your signature |\n");
    printf("|----------------------|\n");
    printf(">>> ");

    if(scanf("%s", params->signature) < 0) params->signature[0] = '\0';
}

void check_flag(flag_params params) {
    char* flag = gen_flag(params);  // Make sure gen_flag returns a dynamically allocated string

    // Check if flag is NULL before proceeding
    if (flag == NULL) {
        printf("Failed to generate flag.\n");
        return;
    }

    // Do check logic here
    bool pass = true;
    char* flag_base = strtok(flag, "-");
    char* sig_a = strtok(NULL, "-");
    char* sig_b = strtok(NULL, "-");
    
    // Print the sections if they exist
    if (flag_base) printf("%s\n", flag_base);
    if (sig_a) printf("%s\n", sig_a);
    if (sig_b) printf("%s\n", sig_b);

    // Check if flag_base contains "decide"
    if (flag_base == NULL || strstr(flag_base, "decide") == NULL) {
        pass = false;
    }

    // Validate sig_a and perform MD5 hash check
    if (pass && sig_a) {
        uint8_t hash[16];
        MD5(sig_a, strlen(sig_a), hash);  // Ensure MD5 is defined and working
        if (memcmp(hash, signature_a_hash, 16) != 0) {
            pass = false;
        }
    } else {
        pass = false;  // If pass is true but sig_a is NULL, set pass to false
    }

    // Validate sig_b
    if (pass && sig_b) {
        if (strcmp(sig_b, "chompy") != 0) {
            pass = false;
        }
    } else {
        pass = false;  // If pass is true but sig_b is NULL, set pass to false
    }

    // Output result
    printf("sun{%s} %s\n", flag, pass ? "is Chompy's favorite flag! Great work." : "isn't Chompy's favorite, but it's yours and that's what matters.");

    // Free the dynamically allocated memory if applicable
    free(flag);
}

int menu(flag_params* params) {
    print_menu(*params);

    char user_sel = getchar();
    while (getchar() != '\n');
    switch (user_sel)
    {
    case '1':
        length_menu(params);
        break;
    case '2':
        genre_menu(params);
        break;
    case '3':
        vibe_menu(params);
        break;
    case '4':
        signature_menu(params);
        break;
    case 'x':
        return 0;
    }

    return 1;
}

int main() {
    int run_menu;
    flag_params params = {
        .length = 0,
        .genre = 0,
        .vibe = 0,
    };

    do {
        run_menu = menu(&params);
    } while (run_menu);
    
    return 0;
}