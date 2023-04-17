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

int main() {
    FILE *password_file = fopen("password.txt", "r");
    char password[MAX_PASSWORD_LEN] = "";
    fscanf(password_file, "%s", password);
    xor(password, XOR_CONSTANT);
    char user_input[MAX_PASSWORD_LEN] = "";
    do {
        printf("Enter password: ");
        scanf("%s", user_input);
    } while (strcmp(password, user_input) != 0);

    printf("You are logged in!\n");
    return 0;
}
