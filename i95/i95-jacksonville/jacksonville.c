#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

unsigned int get_uint(void) {
	char buf[20];

	fgets(buf, sizeof(buf) - 1, stdin);
	return (unsigned int)atoi(buf);
}

void bug() {
	char inp[100];

	puts("bug");

	gets(inp);
}

void leak() {
	int ptr;
	
	puts("leak");

	ptr = get_uint();

	printf("0x%lx\n", *((long *)ptr));
}

void main() {
	uint choice;
	while (true) {
		fflush(stdin);
		fflush(stdout);

		puts("Input Menu Option");

		choice = 5;

		choice = get_uint();

		if (choice == 0) leak();

		else if (choice == 1) bug();

		else if (choice == 2) return;
	}
}

