#ifndef INC_1_CRYPTANALYSIS_UTILS_H
#define INC_1_CRYPTANALYSIS_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>

#include "data.h"

int wchar_to_array_index(wchar_t c);
int absolute_index(int i, int array_size);
void error_exit(const char *message);
int most_frequent_value(const int *arr, int size);
void sort_indexes(const double arr[], int *indexes);
void quit();


#endif //INC_1_CRYPTANALYSIS_UTILS_H
