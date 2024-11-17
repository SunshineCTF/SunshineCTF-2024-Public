#include <stdio.h>
#include <stdlib.h>

long get_inp(void) {
	char buf[20];
	long inp;

	fgets(buf, sizeof(buf) - 1, stdin);
	//puts("More Input");
	return atoll(buf);
}

void win() {
	system("/bin/sh");
}

void func0() {
	char inp[20];
	char *p0, *p1;
	long size, index, val; 

	puts("Do you want a leak? ");
	val = get_inp();
	if (val);
		printf("%p\n", inp);

	puts("Enter chunk size: ");
	size = get_inp();

	p0 = malloc(size);
	printf("Here you go: 0x%lx\n", *((long *)p0 + 3));

	puts("Let's fill that buffer up...\n");
	puts("Index: ");
	index = get_inp();
	puts("Value: ");
	val = get_inp();
	*(((long *)p0) + index) = val;

	puts("The chunk is still hungry... let's fill it up some more!\n");
	puts("Index: ");
	index = get_inp();
	puts("Value: ");
	val = get_inp();
	*(((long *)p0) + index) = val;

	p1 = malloc(size);

	puts("Value 1: ");
	val = get_inp();
	*(((long *)p1) + 0) = val;

	puts("Value 2 - ");
	val = get_inp();
	*(((long *)p1) + 1) = val;

	puts("Value 3 -> ");
	val = get_inp();
	*(((long *)p1) + 2) = val;
}

void main() {
	puts("\nWelcome to Heap Land - where all your heap allocations come true <3\n");
	func0();
}