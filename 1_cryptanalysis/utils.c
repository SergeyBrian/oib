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
