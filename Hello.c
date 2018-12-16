#include <stdio.h>
int main(){
	char a[4096];
	scanf("%[^\n]%*c",a);
	printf("%s",a);
	return 0;
}
