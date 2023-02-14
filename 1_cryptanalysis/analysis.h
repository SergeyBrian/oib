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
    wchar_t *decoded_string;
    double frequencies[ALPHABET_SIZE];
    int key[ALPHABET_SIZE];
} analysis_state;


void measure_letters_frequency();
void find_most_frequent_ngram(int n, wchar_t *result);
wchar_t *apply_key();
void generate_key(wchar_t *method, wchar_t key[ALPHABET_SIZE][2]);
void analysis_init();
wchar_t *get_source_string();
wchar_t *get_decoded_string();
double *get_frequencies();
void match_frequencies(const double freq1[], const double freq2[], int matches[]);
int *get_key_ptr();

#endif //INC_1_CRYPTANALYSIS_ANALYSIS_H
