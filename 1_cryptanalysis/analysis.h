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

typedef enum {
    NO_MATCH,
    MATCH,
    STRICT_MATCH
} match_type;

typedef struct {
    wchar_t *string;
    wchar_t *decoded_string;
    wchar_t **words;
    wchar_t **decoded_words;
    wchar_t **decoded_words_sorted;
    wchar_t **most_frequent_bigrams;
    wchar_t **decoded_bigrams;
    wchar_t **wordlist;
    int wordlist_length;
    int words_count;
    double frequencies[ALPHABET_SIZE];
    double *bigrams_frequencies;
    int key[ALPHABET_SIZE];
    int ready;
    double min_valid_words;
    int *key_history[HISTORY_SIZE];
    int history_index;
} analysis_state;


void measure_letters_frequency();

void find_most_frequent_ngram(int n, wchar_t *result);

wchar_t *apply_key();

void apply_key_to_str(const wchar_t *string, wchar_t *decoded_string);

void generate_key(wchar_t *method, wchar_t key[ALPHABET_SIZE][2]);

void analysis_init();

wchar_t *get_source_string();

wchar_t *get_decoded_string();

wchar_t **get_decoded_bigrams();

double *get_frequencies();

wchar_t **get_words();

wchar_t **get_decoded_words();

wchar_t **get_words_sorted_by_decoded_letters();

int get_words_count();

void set_words_count(int count);

void match_frequencies(const double freq1[], const double freq2[], int matches[]);

int *get_key_ptr();

void generate_mask(wchar_t *string, wchar_t *mask);

void generate_key_from_matches(const wchar_t *encoded, const wchar_t *decoded);

match_type does_match_mask(const wchar_t *string, const wchar_t *mask);

int is_key_valid();

void add_key_to_history();

void undo_key_change();

void redo_key_change();

double *get_bigrams_frequencies();

wchar_t **get_bigrams();

void set_wordlist(wchar_t **wordlist, int len);

void auto_generate_key();

#endif //INC_1_CRYPTANALYSIS_ANALYSIS_H
