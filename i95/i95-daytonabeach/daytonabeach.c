#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void vuln() {
	char *prompt;
	char input[132];
	
	puts("You've taken a pit-stop at the Daytona 500!!");
	prompt = malloc(0x100);
	strcpy(prompt, "Who do you think'll win? ");

	gets(input);
	puts(prompt);
}

void main() {
	vuln();
}

