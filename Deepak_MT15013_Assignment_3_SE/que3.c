#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void function1(const unsigned char *ptr)
{
	printf("Inside function1. Must not return to main()\n");
	char input[4];
	memset(input,0,4);
	strcpy(input,ptr);
	printf("String: %s\n",input);
}

void target()
{
	printf("This is the target function\n");
	exit(0);
}

int main(int argc, unsigned char **argv)
{
	printf("Inside Main\n");
	unsigned char buff[256];
	memset(buff,0,256);
	gets(buff);
	printf("Input string:");
	gets(buff);
	function1(buff);
	printf("back to main!\n"); 
	return 0;
}