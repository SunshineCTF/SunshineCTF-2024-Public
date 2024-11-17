#include <stdio.h>
#include <stdlib.h>

void func0() {
	char inp[148];
	printf("Speed limit: %p\n", inp);
	gets(inp);
}

void main() {
	fflush(stdin);
	fflush(stdout);
	func0();
}

