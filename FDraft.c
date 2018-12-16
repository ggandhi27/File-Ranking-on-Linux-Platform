#include<stdio.h>
#include<conio.h>
int main()
{
	char ch;
	FILE *fp;
	fp = fopen("File.txt","W");
	printf("Enter Data");
	while((ch = getchar())!= EOF)
	{
		putc(ch,fp);
	}
	fclose(fp);
	fp=fopen("File.txt","r");
	while((ch=getc())!=EOF)
	{
		printf("%c",ch);
	}
	fclose(fp);
}
