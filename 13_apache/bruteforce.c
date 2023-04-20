#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PASSWORD_LENGTH 4
#define MAX_ENCODED_LENGTH 8
#define MAX_INPUT_LENGTH 30

#define ALPHABET_LENGTH 63

char alphabet[ALPHABET_LENGTH] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";

unsigned char *base64_encode(const unsigned char *input, size_t len, int wrap);


int main() {
    char input[MAX_INPUT_LENGTH] = "";
    printf("Enter base64 encoded password: ");
    scanf("%s", input);
    if (strlen(input) > MAX_ENCODED_LENGTH) {
        printf("Password is too long (max %d symbols)", MAX_PASSWORD_LENGTH);
        return 1;
    }

    unsigned char password[MAX_PASSWORD_LENGTH + 1] = {0};

    for (int i = 0; i < ALPHABET_LENGTH; i++) {
        for (int j = 0; j < ALPHABET_LENGTH; j++) {
            for (int k = 0; k < ALPHABET_LENGTH; k++) {
                for (int m = 0; m < ALPHABET_LENGTH; m++) {
                    password[0] = alphabet[i];
                    password[1] = alphabet[j];
                    password[2] = alphabet[k];
                    password[3] = alphabet[m];

                    for (int n = 0; MAX_PASSWORD_LENGTH - n >= 0; n++) {
                        unsigned char *encoded_password = base64_encode(password, MAX_PASSWORD_LENGTH - n, 0);
                        if (strcmp(input, (char*)encoded_password) == 0) {
                            password[MAX_PASSWORD_LENGTH - n] = '\0';
                            printf("Decoded password: %s\n", password);
                            return 0;
                        }
                    }
                }
            }
        }
    }

    return 0;
}



char b64table[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "abcdefghijklmnopqrstuvwxyz"
                    "0123456789+/";

unsigned char *base64_encode(const unsigned char *input, size_t len, int wrap) {
    unsigned char *output, *p;
    size_t i = 0, mod = len % 3;

    p = output = (unsigned char *) malloc(((len / 3) + (mod ? 1 : 0)) * 4 + 1);
    if (!p) return 0;

    while (i < len - mod) {
        *p++ = b64table[input[i++] >> 2];
        *p++ = b64table[((input[i - 1] << 4) | (input[i] >> 4)) & 0x3f];
        *p++ = b64table[((input[i] << 2) | (input[i + 1] >> 6)) & 0x3f];
        *p++ = b64table[input[i + 1] & 0x3f];
        i += 2;
        if (wrap && !(i % 57)) *p++ = '\n';
    }
    if (!mod) {
        if (wrap && i % 57) *p++ = '\n';
        *p = 0;
        return output;
    } else {
        *p++ = b64table[input[i++] >> 2];
        *p++ = b64table[((input[i - 1] << 4) | (input[i] >> 4)) & 0x3f];
        if (mod == 1) {
            *p++ = '=';
            *p++ = '=';
            if (wrap) *p++ = '\n';
            *p = 0;
            return output;
        } else {
            *p++ = b64table[(input[i] << 2) & 0x3f];
            *p++ = '=';
            if (wrap) *p++ = '\n';
            *p = 0;
            return output;
        }
    }
}
