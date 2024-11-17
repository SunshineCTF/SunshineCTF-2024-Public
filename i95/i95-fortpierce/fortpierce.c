#include <stdio.h>
#include <stdlib.h>

void get_flag() {
	FILE *fp;
	char flag[100];

	fp = fopen("flag.txt", "r");

	if (fp == NULL) {
		puts("There was an issue opening the file.");
		return;
	}

	fgets(flag, sizeof(flag) - 1, fp);

	puts(flag);

	fclose(fp);

}

void main() {
	void (*ptr)(void);
	char inp[100];
	char target[100];

	fflush(stdout);
	fflush(stdin);
	
	printf("Welcome to Fort Pierce... please store your munitions here: ");
	ptr = 0x00;

	memset(inp, 0x00, 100);
	memset(target, 0x00, 100);

	fgets(inp, sizeof(inp) + 40, stdin);

	if (inp[0] != 'f') exit(0);

	if (inp[50] != 'u') exit(0);

	if (inp[88] != 'z') exit(0);

	if (inp[12] != 'z') exit(0);

	if (inp[9] != 'y') exit(0);

	if (inp[79] != 's') exit(0);

	if (inp[47] != 'o') exit(0);

	if (inp[40] != 'c') exit(0);

	if (inp[90] != 'k') exit(0);

	if (inp[32] != 's') exit(0);

	ptr = (*(long *)target ^ 0x8);

	if (ptr != 0) {
		ptr();
	}
}



