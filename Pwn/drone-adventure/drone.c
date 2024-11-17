#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>

__attribute__((constructor)) void ignore_me() {
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
}

int ARMX_VAL = 0;
bool SAFE = 0;

void handleARMX(int position); /* bug where ARMX - value leads to corruption, exploitable */
void handleBRKS(char engage); /* bug where value not E.... leads to second copy; only one-gadget-> dep_feedback */
void handleCAMO(char mode); /*bug where CAMO == (I) followed by multiple other letters, tripggers dep_feedback */
void handleCLAW(char openClose); /*bug where OC triggers bug report... leads to second copy; only one-gadget-> dep_feedback */
void handleDEPL(char module);
void handleLIGH(char intensity);
void handleMOVE(char direction, int distance);
void handleRECD(int duration); /* bug where second extension is improperly handled, only one gadget allowed */
void handleSAMP(int sampleSize); /* bug where float triggers bug report triggers one gadget */
void handleSAFE(char state); /* safe */
void handleSCAN(char scanType);
void handleSENS(char type);
void handleSIGN(char *message); /* bug where first message contains uppercase, gets second message */
void handleSTOP();  /*bug where stop reason is blank; triggers vulnerable log_resason... */
void handleTURN(float angle); 
void handleTRNS(float frequency);


__attribute__((naked)) void gadget2() {
    __asm__ volatile (
        "pop %rax\n"   
        "pop %rdx\n"
        "pop %rcx\n"
        "ret\n"        
    );
}

void log_reason(char *reason) {
    char explanation[32];
    memset(explanation,'\0',32);
    printf("<<< Did not get reason for stopping\n");
    printf("Enter reason for stopping >>> ");
    read(0,&explanation,256);
    printf("<<< Rover stopped due to: %s\n", explanation);
}

__attribute__((visibility("default"))) void bug_report(char *bug) {
    char buf[16];
    strcpy(buf,bug);
    printf("<<< Bug report submitted.");
}

__attribute__((naked)) void gadget1() {
    __asm__ volatile (
        "pop %rax\n"   
        "ret\n"        
    );
}


void check_filename() {
    char *second_dot = NULL;
    int dot_count = 0;
    char second_extension[8];
    char filename[1000];
    printf("Enter filename for the recording >>> ");
    if (read(0,&filename,sizeof(filename))!=NULL) { 
        for (int i = 0; i <= 1000; i++) {
            if (filename[i]=='\n') {
                filename[i]='\0';
            }
        }
        printf("<<< Saved file: %s\n", filename);

    } else {
        printf("<<< Error saving file.\n");
    }

    for (char *p = filename; *p != '\0'; p++) {
        if (*p == '.') {
            dot_count++;
            if (dot_count == 2) {
                second_dot = p + 1;  
                break;
            }
        }
    }
    if (second_dot != NULL && *second_dot != '\0') {
        strcpy(second_extension, second_dot);
        printf("<<< Warning, did you mean to save with %s extension\n", second_extension);       
    } 
}

__attribute__((naked)) void gadget3() {
    __asm__ volatile (
        "pop %rdi\n"   
        "pop %rsi\n"
        "syscall\n"        
    );
}


__attribute__((visibility("default"))) void deprecated_feedback() {
    if (SAFE==1) {
        char buf[256];
        printf("Leave feedback comments for the developers >>> ");
        read(0,&buf,512);
    }
    else {
        printf("<<< Feedback only available in unSAFE state\n");
    }
}


__attribute__((naked)) void gadget4() {
    __asm__ volatile (
        "pop %rax\n"   
        "pop %rsi\n"
        "syscall\n"        
    );
}


void executeCommand(char *command, char *param) {
    char buffer[16];

    if (strncmp(command, "RECD",4) == 0) {
        int duration = atoi(param);
        if (duration >= 1 && duration <= 300) {
            handleRECD(duration);
            check_filename();            
        } else {
            printf("<<< Invalid duration for RECD. Must be between 1 and 300 seconds.\n");
        }

    } else if (strncmp(command, "MOVE",4) == 0) {
        char direction = param[0];
        int distance = atoi(strchr(param, ' ') + 1);
        if ((direction == 'F' || direction == 'B') && distance >= 1 && distance <= 1000) {
            handleMOVE(direction, distance);
            char terrain[20];
            printf("Enter terrain type (e.g., rocky, sandy) >>> ");
            fgets(terrain, sizeof(terrain), stdin);
            terrain[strcspn(terrain, "\n")] = 0;
            printf("<<< Moving over %s terrain...\n", terrain);
        } else {
            printf("<<< Invalid parameters for MOVE. Direction must be 'F' or 'B', and distance must be between 1 and 1000 meters.\n");
        }
    } else if (strncmp(command, "STOP",4) == 0) {
        handleSTOP();
        char reason[100];
        memset(reason,'\0',100);
        printf("Enter reason for stopping >>> ");
        read(0,&reason,100);
           if (strlen(reason)<2) {
            log_reason(reason);
        }
    } else if (strncmp(command, "TURN",4) == 0) {
        float angle = atof(param);
        if (angle >= -180.0 && angle <= 180.0) {
            handleTURN(angle);
            char direction;
            printf("Enter turn direction (L for left, R for right) >>> ");
            scanf(" %c", &direction);
            printf("<<< Turning %c by %.2f degrees...\n", direction == 'L' ? 'left' : 'right', angle);
        } else {
            printf("<<< Invalid angle for TURN. Must be between -180 and 180 degrees.\n");
        }
    } else if (strncmp(command, "LIGH",4) == 0) {
        char intensity = param[0];
        if (intensity >= '1' && intensity <= '5') {
            handleLIGH(intensity);
            char mode[20];
            printf("Enter light mode (e.g., steady, blinking) >>> ");
            fgets(mode, sizeof(mode), stdin);
            mode[strcspn(mode, "\n")] = 0;
            printf("<<< Light set to %s mode at intensity level %c...\n", mode, intensity);
        } else {
            printf("<<< Invalid intensity for LIGH. Must be between '1' and '5'.\n");
        }
    } else if (strncmp(command, "CAMO",4) == 0) {
        char mode = param[0];
        if (mode == 'N' || mode == 'I' || mode == 'T') {
            if (strlen(param) > 1) {
                printf("<<< Multi-Mode camera no longer supported, leave feedback if your team needs it.\n");
                deprecated_feedback();

            }
            else {
                handleCAMO(mode);
                int resolution;
                printf("Enter resolution (e.g., 720, 1080) >>> ");
                scanf("%d", &resolution);
                printf("<<< Camera mode set to %c with resolution %d...\n", mode, resolution);
                }

        } else {
            printf("<<< Invalid mode for CAMO. Must be 'N', 'I', or 'T'.\n");
        }
    } else if (strncmp(command, "SIGN",4) == 0) {
        handleSIGN(param);
    } else if (strncmp(command, "SENS",4) == 0) {
        char type = param[0];
        if (type == 'T' || type == 'H' || type == 'P') {
            handleSENS(type);
        } else {
            printf("<<< Invalid sensor type for SENS. Must be 'T', 'H', or 'P'.\n");
        }
    } else if (strncmp(command, "ARMX",4) == 0) {
        int position = atoi(param);
        if (position <= 10) {
            handleARMX(position);
        } else {
            printf("<<< Invalid position for ARMX. Input must be between 1 and 10.\n");
        }
    } else if (strncmp(command, "CLAW",4) == 0) {
        char openClose = param[0];

        if (strchr(param, 'O') && strchr(param, 'C')) {
            printf("<<< Both open and close not permitted simultaneously\n");
            printf("Submit bug report (Y|N) >>> ");
            char bug[3];
            read(0,&bug,3);
            if (bug[0]=='Y') {
                bug_report(param);
            }
        }

        else if (openClose == 'O' || openClose == 'C') {
                handleCLAW(openClose);
        } else {
            printf("<<< Invalid value for CLAW. Must be 'O' (Open) or 'C' (Close).\n");
        }
    } else if (strncmp(command, "SCAN",4) == 0) {
        char scanType = param[0];
        if (scanType == 'R' || scanType == 'S' || scanType == 'D') {
            handleSCAN(scanType);
        } else {
            printf("<<< Invalid scan type for SCAN. Must be 'R', 'S', or 'D'.\n");
        }
    } else if (strncmp(command, "SAMP",4) == 0) {
        for (int j=0;j<strlen(param);j++) {
            if (param[j]=='.') {
                printf("<<< Sample size must be an integer not float.\n");
                printf("<<< Submitting bug report, attempting to continue");
                bug_report(param);
                break;
            }
        }

        int sampleSize = atoi(param);
        if (sampleSize >= 1 && sampleSize <= 500) {
            handleSAMP(sampleSize);
        } else {
            printf("<<< Invalid sample size for SAMP. Must be between 1 and 500 grams.\n");
        }

    }
    else if (strncmp(command, "TRNS",4) == 0) {
        float frequency = atof(param);
        if (frequency >= 1.0 && frequency <= 100.0) {
            handleTRNS(frequency);
        } else {
            printf("<<< Invalid frequency for TRNS. Must be between 1.0 and 100.0 MHz.\n");
        }
    } else if (strncmp(command, "BRKS",4) == 0) {
        char engage = param[0];
        if (engage == 'E' || engage == 'D') {
            handleBRKS(engage);
        } else {
            printf("<<< Invalid value for BRKS. You entered %s Must be 'E' (Engage) or 'D' (Disengage).\n",param);
            printf("<<< Submitting bug report");
            bug_report(param);
        }
    } else if (strncmp(command, "DEPL",4) == 0) {
        char module = param[0];
        if (module >= 'A' && module <= 'D') {
            handleDEPL(module);
        } else {
            printf("<<< Invalid module for DEPL. Must be 'A', 'B', 'C', or 'D'.\n");
        }
    } else if (strncmp(command, "SAFE",4) == 0) {
        char state = param[0];
        if (state == 'Y' || state == 'N') {
            handleSAFE(state);
        } else {
            printf("<<< Invalid state for SAFE. Must be 'Y' (Yes) or 'N' (No).\n");
        }
    } else {
        printf("<<< Unknown command: %s\n", command);
    }
}

__attribute__((naked)) void gadget7() {
    __asm__ volatile (
        "pop %rdi\n"
        "ret\n"        
    );
}

void handleRECD(int duration) {
    printf("<<< Recording video for %d seconds...\n", duration);
}

void handleMOVE(char direction, int distance) {
    printf("<<< Moving %c for %d meters...\n", direction == 'F' ? 'forward' : 'backward', distance);
}

void handleSTOP() {
    printf("<<< Stopping the rover...\n");
}

void handleTURN(float angle) {
    printf("<<< Turning the rover by %.2f degrees...\n", angle);
}

__attribute__((naked)) void gadget8() {
    __asm__ volatile (
        "pop %rdx\n"
        "ret\n"        
    );
}

void handleLIGH(char intensity) {
    printf("<<< Setting light intensity to level %c...\n", intensity);
}

__attribute__((naked)) void gadget9() {
    __asm__ volatile (
        "pop %rcx\n"
        "ret\n"        
    );
}
void handleCAMO(char mode) {
    const char *modeName = (mode == 'N') ? "Normal" : (mode == 'I') ? "Infrared" : "Thermal";
    printf("<<< Switching camera to %s mode...\n", modeName);
}

void to_uppercase(char *str) {
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

void no_shell4() {
    printf("<<< no /bin/sh\0");
}

void handleSIGN(char *message) {
    char buf[256];
    memset(buf,'\0',256);
    for (int i=0; i<=256; i++) {
         printf("Char[%i]: %c\n",i,message[i]);
        if (message[i] >= 'A' && message[i] <= 'Z') {
            if (message[i]=='\0' & message[i]=='\n') break;
            printf("<<< Warning: uppercase letters not permitted on SIGN\n");
            printf("Re-enter message >>> ");
            gets(buf);
            break;
        } 
    }
    strcpy(message,buf);
    printf("<<< Displaying message on sign: %s\n", message);
}

void handleSENS(char type) {
    const char *sensorType = (type == 'T') ? "Temperature" : (type == 'H') ? "Humidity" : "Pressure";
    printf("<<< Activating %s sensor...\n", sensorType);
}

void handleARMX(int position) {
    char buf[256];
    int REF_ARMX = ARMX_VAL+10;
    ARMX_VAL = ARMX_VAL + position;

    printf("<<< Moving arm to position %d..., Current Setting: %d\n", position, ARMX_VAL);
   if (ARMX_VAL > REF_ARMX) {
        printf("<<< Entering Diagnostic Mode, arm increased by more than 10.\n");
        printf("Please leave your spacesec contact so we can replicate the fault >>> ");
        read(0,&buf,512);
    }
}

void handleCLAW(char openClose) {
    printf("<<< %s the claw...\n", openClose == 'O' ? "Opening" : "Closing");
}

void handleSCAN(char scanType) {
    const char *scanTypeName = (scanType == 'R') ? "Radar" : (scanType == 'S') ? "Sonar" : "Depth";
    printf("<<< Performing %s scan...\n", scanTypeName);
}

void handleSAMP(int sampleSize) {
    printf("<<< Collecting a sample of %d grams...\n", sampleSize);
}

void handleTRNS(float frequency) {
    printf("<<< Transmitting data at %.2f MHz...\n", frequency);
}

void handleBRKS(char engage) {
    printf("<<< %s the brakes...\n", engage == 'E' ? "Engaging" : "Disengaging");
}

__attribute__((naked)) void gadget6() {
    __asm__ volatile (
        "pop %rsi\n"   
        "mov 0x31337, %rax\n"
        "ret\n"        
    );
}



void handleDEPL(char module) {
    printf("<<< Deploying module %c...\n", module);
}

__attribute__((naked)) void gadget5() {
    __asm__ volatile (
        "pop %rax\n"   
        "mov 0x3c, %rax\n"
        "ret\n"        
    );
}


void handleSAFE(char state) {
    printf("<<< Setting safety mode to %s...\n", state == 'Y' ? "Enabled" : "Disabled");
    if (state == 'Y') {
        SAFE = 0;
    }
    else {
        SAFE = 1;
    }
}

void logo() {
    printf("================================================================================\n");
    printf("MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n");
    printf("MMMMMMMMMMWWWWWWWWWWWWWWWWWWWWWWWWWWWWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n");
    printf("MMMMMMMNkc;;;;;;;;;;;;;;;;;;;;;;;;;;;;;ld0WMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n");
    printf("MMMMMMWx.                                ,0MMMMMMMMMMMWNWMMMMMMMMMMMXxooxXMMMMMM\n");
    printf("MMMMMMWo        .;cc:,.      .;ccccc:.   .OMMMMMMMMMW0:.;xNMMMMMMMMK;    ;0MMMMM\n");
    printf("MMMMMMWo     .:ONWMMMWKo.    'xOOOOOk,   .OMMMMMMMMWO'    ,xNMMMMMWx.    'OMMMMM\n");
    printf("MMMMMMWo    .oNMWOooxXMW0,    .......    .OMMMMMMMMK;       ,xNMW0l..';:l0WMMMMM\n");
    printf("MMMMMMWo    ,KMMk.   cNMWo   'xOOOOOx,   .OMMMMMMMMx.         ,oc..,kNWMMMMMMMMM\n");
    printf("MMMMMMWo    'OMM0:..'xWMNl   .;ccccc:.   .OMMMMMMMMd.            .dNMMMMMMMMMMMM\n");
    printf("MMMMMMWo     ;0WMNKKXWMXo.   .,;;;;;,.   .OMMMMMMMMO.             ,xNMMMMMMMMMMM\n");
    printf("MMMMMMWo      .cx0KKKko'     'x00000k,   .OMMMMMMMMNl               ,xNMMMMMMMMM\n");
    printf("MMMMMMWo          ...          .....     .OMMMMMMMMMXc                ,xNMMMMMMM\n");
    printf("MMMMMMMO,                              ..cXMMMMMMMMMMNd'                :KMMMMMM\n");
    printf("MMMMMMMMXOxxxc.                   .oxxx0XNMMMMMMMMMMMMWXd;.           .,dXMMMMMM\n");
    printf("MMMMMMMMMMMMMk.                   ;XMMMMMMMMMMMMMMMMMMMNKXKkoc;,,,;:lx0NMMMMMMMM\n");
    printf("MMMMMMMMMMMMMK:...................oNMMMMMMMMMMMMMMMMMMMO,'cokKWWWWWMMMMMMMMMMMMM\n");
    printf("MMMMMMMMMMMMMMNXKKKK000000000KKKKXWMMMMNKkooooooxXMMMMMk.    :XMMMMMMMMMMMMMMMMM\n");
    printf("MMMMMMMMMMMMMMMMMMWx;;;;;;;;c0MMMMMMMMMk:.      .xMMMMMk.    ;XMMMMMMMMMMMMMMMMM\n");
    printf("MMMMMMMMMMMMMMMMMMNc        .xMMMMMMMMMk;.      .xMMMMMk.    ;XMMMMMMMMMMMMMMMMM\n");
    printf("MMMMMNOoooooooooool'         ;ooooooooo;.        ,ooooo;     .cooooooooodKWMMMMM\n");
    printf("MMMMMk.                                                                  :NMMMMM\n");
    printf("MMMMMx.                                                                  ;XMMMMM\n");
    printf("MMMMM0c,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,;dNMMMMM\n");
    printf("MMMMMNK000000000000000000000000000000000000000000000000000000000000000000XWMMMMM\n");
    printf("MMMMMk,..................................................................cXMMMMM\n");
    printf("MMMMMx.                                                                  ;XMMMMM\n");
    printf("MMMMMx.                                                                  ;XMMMMM\n");
    printf("MMMMMx.            .',,'.                             ..',,'.            ;XMMMMM\n");
    printf("MMMMMx.        .;d0XNWWNKOo,                       .:x0XNWWNKkl'         ;XMMMMM\n");
    printf("MMMMMx.       ;OWMMNK00XWMMNd.                    :0WMWNK00XWMMXo.       ;XMMMMM\n");
    printf("MMMMMk.      :KMMXd,....:kWMWO.                  cXMMXo'. ..c0WMWx.      :XMMMMM\n");
    printf("MMMMMXl...  .kMMNl       .kWMWl   ............  'OMMXc       'OMMNc   ..,kWMMMMM\n");
    printf("MMMMMMWXK0c .OMMX:       .dWMWo .oKKKKKKKKKKKO; '0MMK;       .kMMNc .xKKNMMMMMMM\n");
    printf("MMMMMMMMMMk. oNMW0;     .lXMMK; ,KMMMMMMMMMMMWd..dWMWk,     .dNMM0' :XMMMMMMMMMM\n");
    printf("MMMMMMMMMMNo..oNMMNOdooxKWMWK: .kWMMMMMMMMMMMMXc .xNMMXOdloxKWMW0; 'OMMMMMMMMMMM\n");
    printf("MMMMMMMMMMMNd..,xKWMMMMMMN0o. ,kWMMMMMMMMMMMMMMXl..;kXWMMMMMWNOl..;0WMMMMMMMMMMM\n");
    printf("MMMMMMMMMMMMW0l'..;coool:,..,dXMMMMMMMMMMMMMMMMMW0c...;coool:'..;xNMMMMMMMMMMMMM\n");
    printf("MMMMMMMMMMMMMMMXOoc;''',;cd0NMMMMMMMMMMMMMMMMMMMMMWXko:,''',:lxKWMMMMMMMMMMMMMMM\n");
    printf("MMMMMMMMMMMMMMMMMMMWNNNWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNNNWWMMMMMMMMMMMMMMMMMMM\n");
    printf("MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n");
    printf("================================================================================\n");
    printf(" Space Security Industries DroverOS v1.337 Beta (MarsPwn)                       \n");
    printf("================================================================================\n");

}

int main() {

    logo();
    char command[5];
    char param[256];
    while (1) {
        memset(command,'\0',5);
        memset(param,'\0',256);
        printf("Enter command verb >>> ");
        read(0,&command,sizeof(command));
        printf("Enter command params >>> ");
        read(0,&param,256);
        executeCommand(command, param);
    }
    return 0;
}

