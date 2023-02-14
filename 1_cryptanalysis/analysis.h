#ifndef INC_1_CRYPTANALYSIS_ANALYSIS_H
#define INC_1_CRYPTANALYSIS_ANALYSIS_H

#include <wchar.h>
#include <wctype.h>
#include <math.h>

#include "fs.h"
#include "utils.h"

typedef struct {
    wchar_t *string;
    int count;
} ngram;

typedef struct {
    wchar_t *string;
    wchar_t *decoded_string;
    wchar_t **words;
    wchar_t **decoded_words;
    int words_count;
    double frequencies[ALPHABET_SIZE];
    int key[ALPHABET_SIZE];
    int ready;
} analysis_state;


void measure_letters_frequency();
void find_most_frequent_ngram(int n, wchar_t *result);
wchar_t *apply_key();
void apply_key_to_str(const wchar_t *string, wchar_t *decoded_string);
void generate_key(wchar_t *method, wchar_t key[ALPHABET_SIZE][2]);
void analysis_init();
wchar_t *get_source_string();
wchar_t *get_decoded_string();
double *get_frequencies();
wchar_t **get_words();
wchar_t **get_decoded_words();
int get_words_count();
void match_frequencies(const double freq1[], const double freq2[], int matches[]);
int *get_key_ptr();
void generate_mask(wchar_t *string, wchar_t *mask);
int does_match_mask(const wchar_t *string, const wchar_t *mask);

#endif //INC_1_CRYPTANALYSIS_ANALYSIS_H
