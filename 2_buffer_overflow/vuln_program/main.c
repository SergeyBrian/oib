#include <string.h>

int main(int argc, char **argv) {
    char buff[500];
    if (argc > 1) {
        strcpy(buff, argv[1]);
    }
    return 0;
}
