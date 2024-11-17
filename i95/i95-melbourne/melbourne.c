#include <stdio.h>
#include <string.h>

void main() {
	FILE *fp;
	char flag[100], inp[100], target[100];

	fflush(stdout);
	fflush(stdin);

	fgets(inp, sizeof(inp) + 40, stdin);

	if (strncmp(target, "0xdeadbeef", 10) != 0) return;


	fp = fopen("flag.txt", "r");

	if (fp == NULL) {
		puts("There was an issue opening the file.");
		return;
	}

	fgets(flag, sizeof(flag) - 1, fp);

	printf("Flag is: %s\n", flag);

	fclose(fp);
}


