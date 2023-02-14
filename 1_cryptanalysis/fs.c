#include "fs.h"

FILE *input_file;

int open_file(char *filename) {
    input_file = fopen(filename, "r");
    return (input_file != NULL);
}

wchar_t *readfile() {
    int size = MAX_TEXT_LENGTH;

    wchar_t *buff = (wchar_t *) malloc(sizeof(wchar_t) * size);

    fgetws(buff, size, input_file);
    if (buff[wcslen(buff) - 1] == '\n') buff[wcslen(buff) - 1] = '\0';

    unsigned int l = wcslen(buff);
    for (int i = 0; i < l; i++) {
        if (buff[i] == L'ё' || buff[i] == L'Ё') buff[i] = L'е';

        buff[i] = (wchar_t) towupper(buff[i]);
    }

    return buff;
}
