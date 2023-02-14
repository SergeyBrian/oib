#include "utils.h"
#include "ui.h"


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

void quit() {
    ui_quit();
    exit(0);
}

int wchar_to_array_index(wchar_t c) {
    return (int) (c) - FIRST_ALPHABET_LETTER_WCHAR_VALUE;
}

int absolute_index(int i, int array_size) {
    i = i % array_size;
    if (i < 0) {
        i = array_size - i - 2;
    }
    return i;
}

void sort_indexes(const double arr[], int *indexes) {

    double temp;
    int tempIndex;


    // Initialize the indexes array
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        indexes[i] = i;
    }

    // Sort the indexes based on the elements of the array
    for (int i = 0; i < ALPHABET_SIZE - 1; i++) {
        for (int j = i + 1; j < ALPHABET_SIZE; j++) {
            if (arr[indexes[j]] > arr[indexes[i]]) {
                tempIndex = indexes[i];
                indexes[i] = indexes[j];
                indexes[j] = tempIndex;
            }
        }
    }
}

void sort_words_by_length(const wchar_t* input_str, wchar_t *words[], wchar_t *words_ptr[]) {
    wchar_t* str_copy = wcsdup(input_str);
    wchar_t* p = str_copy;
    int word_count = 0;

    unsigned int l = wcslen(str_copy);
    for (int i = 0; i < l; i++) {
        if (iswalpha(str_copy[i])) continue;
        str_copy[i] = L' ';
    }

    while (*p != L'\0' && word_count < MAX_WORDS) {
        while (iswspace(*p)) {
            ++p;
        }
        if (*p == L'\0') {
            break;
        }
        wchar_t* start = p;
        while (*p != L'\0' && !iswspace(*p)) {
            ++p;
        }
        *p = L'\0';
        ++p;
        words[word_count] = (wchar_t *) malloc(sizeof(wchar_t) * (wcslen(start) + 1));
        words_ptr[word_count] = words[word_count];
        wcscpy(words[word_count++], start);
    }

    for (int i = 0; i < word_count - 1; ++i) {
        for (int j = i + 1; j < word_count; ++j) {
            if (wcslen(words[i]) > wcslen(words[j])) {
                wchar_t* temp = words[i];
                words[i] = words[j];
                words[j] = temp;
            }
        }
    }

    int unique_count = 0;
    for (int i = 0; i < word_count; ++i) {
        int is_duplicate = 0;
        for (int j = 0; j < unique_count; ++j) {
            if (wcscmp(words[i], words[j]) == 0) {
                is_duplicate = 1;
                break;
            }
        }
        if (!is_duplicate) {
            words[unique_count++] = words[i];
        }
    }


    words[unique_count] = NULL;

    free(str_copy);
}

