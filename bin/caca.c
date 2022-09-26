#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
	char *str = malloc(sizeof(char *) * 1);

	printf("%c", str[300000]);
	return (0);
}
