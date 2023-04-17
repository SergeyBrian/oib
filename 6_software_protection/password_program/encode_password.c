#include <stdio.h>
#include <string.h>

#define MAX_PASSWORD_LEN 100
#define XOR_CONSTANT 4

void xor(char *str, int n) {
 int l = strlen(str);
 for (int i = 0; i < l; i++) {
 str[i] = str[i]^n;
 }
 }

int main (int argc, char **argv) {
	if (argc < 2) return 1;
	FILE *file = fopen("password.txt", "w");
	xor(argv[1], XOR_CONSTANT);
	fprintf(file, "%s", argv[1]);
	return 0;
}
