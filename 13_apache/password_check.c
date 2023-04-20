#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_USERNAME 30
#define MIN_PASSWORD 10
#define MAX_PASSWORD 50

int isPasswordValid(char *password) {
    unsigned int len = strlen(password);

    int hasUppercase = 0;
    int hasLowercase = 0;
    int hasSpecialCharacters = 0;
    int hasNumbers = 0;

    if (len < MIN_PASSWORD) return 0;
    for (int i = 0; i < len; i++) {
        char c = password[i];

        if (isdigit(c)) hasNumbers = 1;
        if (isupper(c)) hasUppercase = 1;
        if (islower(c)) hasLowercase = 1;
        if (!isalnum(c)) hasSpecialCharacters = 1;
    }

    return hasUppercase && hasLowercase && hasSpecialCharacters && hasNumbers;
}

int main() {
    char username[MAX_USERNAME] = {0};
    char password[MAX_PASSWORD] = {0};
    printf("Enter username: ");
    scanf("%s", username);

    do {
        printf("Enter password: ");
        scanf("%s", password);
    } while (!isPasswordValid(password));

    setenv("username", username, 1);
    setenv("password", password, 1);
    system("sudo htpasswd -b /var/www/html/.htpasswd $username $password");//-b for use arguments, which are passed
    return 0;
}
