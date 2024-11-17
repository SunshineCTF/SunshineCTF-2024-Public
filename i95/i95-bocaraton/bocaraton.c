#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void win() {
	system("/bin/sh");
}

bool test() {
	char test[20];
	fgets(test, sizeof(test) - 1, stdin);
	if (strncmp(test, "wambo-jambo", 11) == 0 && *((int *)(test + 11)) == 0xfeedc0de) return true;
	return false;
}

void func0() {
	char inp[116];
	puts("What food and drinks should we bring?");
	fgets(inp, sizeof(inp) - 1, stdin);
	printf(inp);
}

void func1() {
	char inp[116];
	puts("Now what should we do at the beach?");
	gets(inp);
}

void main() {
	printf("Enter Code: ");
	if (!test())
		return;

	puts("It's time for a beach party!!");

	func0();
	func1();

	puts("Uh-oh... party foouul");
}

