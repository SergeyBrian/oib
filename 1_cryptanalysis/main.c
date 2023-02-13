#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <bits/wctype-wchar.h>
#include <math.h>
#include <string.h>

#include "data.h"


typedef enum {
    INPUT_FILE_NAME,
    OUTPUT_FILE_NAME,
    GENERATE
} param;

typedef enum {
    BEGIN = 1,
    MIDDLE = 2,
    END = 3
} letter_position;

typedef struct {
    wchar_t *string;
    int count;
} ngram;

static inline int wchar_to_array_index(wchar_t c) {
    return (int) (c) - FIRST_ALPHABET_LETTER_WCHAR_VALUE;
}

static inline int absolute_index(int i, int array_size) {
    return i % array_size;
}

void error_exit(const char *message) {
    printf("%s", message);
    exit(1);
}

int most_frequent_value(const int *arr, int size) {
    int i, j, max_count = 0, res = arr[0];
    for (i = 0; i < size; i++) {
        int count = 0;
        for (j = 0; j < size; j++) {
            if (arr[i] == arr[j]) {
                count++;
            }
        }
        if (count > max_count) {
            max_count = count;
            res = arr[i];
        }
    }
    return res;
}

void parse_arguments(int argc, char **argv, char **params_ptr) {
    param current_param;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'i':
                    current_param = INPUT_FILE_NAME;
                    break;
                case 'o':
                    current_param = OUTPUT_FILE_NAME;
                    break;
                case 'g':
                    current_param = GENERATE;
                    break;
                default:
                    error_exit("Incorrect command line arguments");
            }
            continue;
        }

        params_ptr[current_param] = &argv[i][0];
    }
}

void open_input_file(char *filename, FILE **input_file) {
    *input_file = fopen(filename, "r");
    if (input_file == NULL) {
        error_exit("Can't open input file");
    }
}

void open_output_file(char *filename, FILE **output_file) {
    *output_file = fopen(filename, "w");
    if (output_file == NULL) {
        error_exit("Can't open output file");
    }
}

wchar_t *readfile(FILE *input_file) {
    int size = 100000;

    wchar_t *buff = (wchar_t *) malloc(sizeof(wchar_t) * size);

    fgetws(buff, size, input_file);
    if (buff[wcslen(buff) - 1] == '\n') buff[wcslen(buff) - 1] = '\0';

    unsigned int l = wcslen(buff);
    for (int i = 0; i < l; i++) {
        if (buff[i] == L'ё' || buff[i] == L'Ё') buff[i] = L'е';

        buff[i] = (wchar_t) towlower(buff[i]);
    }

    return buff;
}

void measure_letters_frequency(wchar_t *string, double frequencies_ptr[ALPHABET_SIZE]){
    unsigned int l = wcslen(string);
    unsigned int letters_count = 0;

    for (int i = 0; i < l; i++) {
        if (!iswalpha(string[i])) {
            continue;
        }

        frequencies_ptr[wchar_to_array_index(string[i])]++;
        letters_count++;
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        frequencies_ptr[i] /= letters_count;
    }
}

void find_most_frequent_ngram(const wchar_t *string, int n, wchar_t *result) {
    unsigned int l = wcslen(string);
    ngram *ngrams[1000] = {0};
    int ngrams_count = 0;

    for (int i = 0; i < l - n; i++) {
        wchar_t *current_ngram = (wchar_t *) malloc(sizeof(wchar_t) * (n+1));
        for (int j = 0; j < n; j++) {
            if (string[i+j] == L' ') continue;
            current_ngram[j] = string[i + j];
        }
        current_ngram[n] = 0;

        if (wcslen(current_ngram) != n) continue;

        int match_found = 0;
        for (int j = 0; j < ngrams_count; j++) {
            if (wcscmp(ngrams[j]->string, current_ngram) == 0) {
                ngrams[j]->count++;
                match_found = 1;
                break;
            }
        }
        if (!match_found) {
            ngrams[ngrams_count] = (ngram *) malloc(sizeof(ngram));
            ngrams[ngrams_count]->string = (wchar_t *) malloc(sizeof(wchar_t) * n);
            wcscpy(ngrams[ngrams_count]->string, current_ngram);
            ngrams[ngrams_count]->count = 1;
            ngrams_count++;
        }

        free(current_ngram);

    }

    int max_count = 0;
    int max_count_index = 0;

    for (int i = 0; i < ngrams_count; i++) {
        if (ngrams[i]->count > max_count) {
            max_count = ngrams[i]->count;
            max_count_index = i;
        }
    }

    wcscpy(result, ngrams[max_count_index]->string);
    for (int i = 0; i < ngrams_count; i++) {
        free(ngrams[i]->string);
        free(ngrams[i]);
    }
}

void apply_key(const wchar_t key[ALPHABET_SIZE][2], wchar_t *string, wchar_t *result) {
    unsigned int l = wcslen(string);
    for (int i = 0; i < l; i++) {
        if (!iswalpha(string[i])) {
            result[i] = string[i];
            continue;
        }

        result[i] = key[wchar_to_array_index(string[i])][1];
    }
}


void generate_key(wchar_t *method, wchar_t key[ALPHABET_SIZE][2]) {
    if (iswdigit(method[0])) {
        int offset = atoi(method);
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            key[i][0] = ALPHABET_RU[i];
            key[i][1] = ALPHABET_RU[absolute_index(i + offset, ALPHABET_SIZE)];
        }
        return;
    }
    if (wcslen(method) != 32) {
        printf("%d\n", wcslen(method));
        error_exit("Given alphabet does not contain 32 letters");
    }


    for (int i = 0; i < ALPHABET_SIZE; i++) {
        key[i][0] = ALPHABET_RU[i];
        key[i][1] = method[i];
    }
}

int main(int argc, char **argv) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    char filename[100] = {0};
    char *params[10] = {0};

    FILE *input_file;
    FILE *output_file;

    if (argc > 1) parse_arguments(argc, argv, params);

    if (params[INPUT_FILE_NAME] == NULL) {
        printf("Enter input file name: ");
        scanf("%s", params[INPUT_FILE_NAME]);
    }

    open_input_file(params[INPUT_FILE_NAME], &input_file);

    wchar_t *string = readfile(input_file);
    if (!wcslen(string)) error_exit("Input file is empty or encoding is incorrect");

    if (params[GENERATE] == NULL) {
        double frequencies[ALPHABET_SIZE] = {0};
        int letters_count = 0;

        measure_letters_frequency(string, frequencies);

        wchar_t most_frequent_letter[1];
        find_most_frequent_ngram(string, 1, most_frequent_letter);

        return 0;
    } else {
        open_output_file(params[OUTPUT_FILE_NAME], &output_file);

        wchar_t key[ALPHABET_SIZE][2];

        FILE *key_file = NULL;

        open_input_file(params[GENERATE], &key_file);
        wchar_t *method = readfile(key_file);

        generate_key(method, key);

        wchar_t result[10000] = {0};

        apply_key(key, string, result);


        fwprintf(output_file,  L"%S", result);
    }



    return 0;
}
