#include "fs.h"

FILE *input_file;
FILE *wordlist_file;

int open_file(char *filename) {
    input_file = fopen(filename, "r");
    return (input_file != NULL);
}

int open_wordlist(char *filename) {
    wordlist_file = fopen(filename, "r");
    return (wordlist_file != NULL);
}

wchar_t *readfile() {
    int size = MAX_TEXT_LENGTH;

    wchar_t *buff = (wchar_t *) malloc(sizeof(wchar_t) * size);

    fgetws(buff, size, input_file);
    if (buff[wcslen(buff) - 1] == '\n') buff[wcslen(buff) - 1] = '\0';

    unsigned int l = wcslen(buff);
    for (int i = 0; i < l; i++) {
        if (buff[i] == L'ё' || buff[i] == L'Ё') {
            buff[i] = L'е';
        }

        buff[i] = (wchar_t) towupper(buff[i]);
    }

    return buff;
}

wchar_t **read_wordlist() {
    if (wordlist_file == NULL) return NULL;
    int words_count;
    int lines_read = 0;
    char  line_buffer[256];

    fscanf(wordlist_file, "%d", &words_count);
    wchar_t **words = (wchar_t **) malloc(sizeof(wchar_t *) * words_count);



    while (fgets(line_buffer, sizeof(line_buffer), wordlist_file)) {
        int line_buffer_len = strlen(line_buffer);
        words[lines_read] = (wchar_t *) malloc(sizeof(wchar_t) * line_buffer_len + 1);
        line_buffer[line_buffer_len - 1] = '\0';
        if (!(*line_buffer)) continue;
        mbstowcs(words[lines_read], line_buffer, strlen(line_buffer) + 1);
        lines_read++;
    }
    set_wordlist(words, words_count);
}
