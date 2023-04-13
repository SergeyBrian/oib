// gcc vuln.c -o vuln -z execstack -fno-stack-protector -no-pie -m32

#include <stdio.h>
#include <string.h>

int copy(int argc, char **argv) {
        if (argc <= 1) return 1;

        char buf[500];
        strcpy(buf, argv[1]);
}

void main(int argc, char **argv) {
        copy(argc, argv);
}

