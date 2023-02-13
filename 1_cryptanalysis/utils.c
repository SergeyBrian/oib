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
    return i % array_size;
}
