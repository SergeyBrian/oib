#ifndef INC_1_CRYPTANALYSIS_UTILS_H
#define INC_1_CRYPTANALYSIS_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>

#include "data.h"
#include "ui.h"

#define MAX_WORDS 500

int wchar_to_array_index(wchar_t c);
int absolute_index(int i, int array_size);
void error_exit(const char *message);
int most_frequent_value(const int *arr, int size);
void sort_indexes(const double arr[], int *indexes);
void sort_words_by_length(const wchar_t *input_str, wchar_t *words[], wchar_t *to_free[]);
void sort_words_by_decoded_letters(const wchar_t *input_str, wchar_t *words[], wchar_t *words_ptr[]);
int count_word_occurrences(const wchar_t *input_str, const wchar_t *word);
int char_index(const char* str, char ch);
int wchar_index(const wchar_t* str, wchar_t ch);
void quit();


#endif //INC_1_CRYPTANALYSIS_UTILS_H
