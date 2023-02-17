#ifndef INC_1_CRYPTANALYSIS_UTILS_H
#define INC_1_CRYPTANALYSIS_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>

#include "data.h"
#include "ui.h"


#define IS_UNIQUE(ARR, LEN, ELEMENT) \
    ({ \
        int UNIQUE_COUNT = 0; \
        for (int I = 0; I < LEN; I++) { \
            if ((ARR)[I] == (ELEMENT)) { \
                UNIQUE_COUNT++; \
            } \
        } \
        UNIQUE_COUNT == 1; \
    })

#define INDEX_OF(ARR, LEN, ELEMENT) \
    ({ \
        int INDEX = -1; \
        for (int I = 0; I < LEN; I++) { \
            if ((ARR)[I] == (ELEMENT)) { \
                INDEX = I; \
                break; \
            } \
        } \
        INDEX; \
    })

int wchar_to_array_index(wchar_t c);

int absolute_index(int i, int array_size);

void error_exit(const char *message);

int most_frequent_value(const int *arr, int size);

void sort_indexes(const double arr[], int *indexes);

void sort_words_by_length(const wchar_t *input_str, wchar_t *words[]);

void sort_words_by_decoded_letters(const wchar_t *input_str, wchar_t *words[]);

void sort_words_by_frequency(const wchar_t *input_str, wchar_t *words[]);

int count_word_occurrences(const wchar_t *word);

int char_index(const char *str, char ch);

int wchar_index(const wchar_t *str, wchar_t ch);

int wcs_index(const wchar_t **str_list, int list_len, wchar_t *str);

int first_word_len(const wchar_t *str);

void quit();

int is_vowel(wchar_t c);


#endif //INC_1_CRYPTANALYSIS_UTILS_H
