#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void append(char buffer[], char *s) {
	int i, len = strlen(buffer);
	for (i = 0; s[i]; i++) {
		buffer[len++] = s[i];
	}
	buffer[len] = 0;
}

int main(int argc, char* args[]) {
	system("gcc -fPIC -shared -o hook.so hook.c -ldl");
	
	char buffer[1000] = "gcc -o test ";
	append(buffer, args[1]);
	append(buffer, " -lpthread");
	//puts(buffer);
	//return 0;
	system(buffer);
	system("LD_PRELOAD=./hook.so ./test");
	system("cat report");
	return 0;
}
