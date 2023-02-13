#ifndef INC_1_CRYPTANALYSIS_ANALYSIS_H
#define INC_1_CRYPTANALYSIS_ANALYSIS_H

#include <wchar.h>
#include <wctype.h>
#include <math.h>

#include "fs.h"
#include "utils.h"

typedef enum {
    BEGIN = 1,
    MIDDLE = 2,
    END = 3
} letter_position;

typedef struct {
    wchar_t *string;
    int count;
} ngram;

typedef struct {
    wchar_t *string;
    double frequencies[ALPHABET_SIZE];
} analysis_state;


void measure_letters_frequency();
void find_most_frequent_ngram(const wchar_t *string, int n, wchar_t *result);
void apply_key(const wchar_t key[ALPHABET_SIZE][2], wchar_t *string, wchar_t *result);
void generate_key(wchar_t *method, wchar_t key[ALPHABET_SIZE][2]);
void analysis_init();
wchar_t *get_source_string();
double *get_frequencies();
void match_frequencies(const double freq1[], const double freq2[], int matches[]);

#endif //INC_1_CRYPTANALYSIS_ANALYSIS_H
