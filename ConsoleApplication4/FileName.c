#include <stdio.h>


main()
{
	int c=0;
	FILE* fp;
	FILE* fopen(char* name, char* mode);
	fp = fopen("C:/Users/hf/source/repos/ConsoleApplication4/ConsoleApplication4/FileName.c", "r");
	while ((c=getc(fp))!=EOF)
	{
		printf("%c",c);
	}
}