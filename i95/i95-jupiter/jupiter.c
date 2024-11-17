#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void win() {
	system("/bin/sh");
}

bool test() {
	char test[20];
	printf("Quiz Time - What's the zipcode of Jupiter, FL? ");
	fgets(test, sizeof(test) - 1, stdin);
	if (strncmp(test, "0xdeadc0de", 10) == 0) return true;
	return false;
}

void func0() {
	char inp[100];
	printf("What's Jupiter's best beach?? ");
	fgets(inp, sizeof(inp) - 1, stdin);
	printf(inp);
}

void main() {
	if (!test()) return;
	puts("Yippee! Now onto the real test...\n");
	func0();

	puts("\nIt's time!");
}

