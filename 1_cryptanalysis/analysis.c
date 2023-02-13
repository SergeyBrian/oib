#include "analysis.h"

analysis_state state;

void measure_letters_frequency(wchar_t *string, double frequencies_ptr[ALPHABET_SIZE]){
    unsigned int l = wcslen(string);
    unsigned int letters_count = 0;

    for (int i = 0; i < l; i++) {
        if (!iswalpha(string[i])) {
            continue;
        }

        frequencies_ptr[wchar_to_array_index(string[i])]++;
        letters_count++;
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        frequencies_ptr[i] /= letters_count;
    }
}

void find_most_frequent_ngram(const wchar_t *string, int n, wchar_t *result) {
    unsigned int l = wcslen(string);
    ngram *ngrams[1000] = {0};
    int ngrams_count = 0;

    for (int i = 0; i < l - n; i++) {
        wchar_t *current_ngram = (wchar_t *) malloc(sizeof(wchar_t) * (n+1));
        for (int j = 0; j < n; j++) {
            if (string[i+j] == L' ') continue;
            current_ngram[j] = string[i + j];
        }
        current_ngram[n] = 0;

        if (wcslen(current_ngram) != n) continue;

        int match_found = 0;
        for (int j = 0; j < ngrams_count; j++) {
            if (wcscmp(ngrams[j]->string, current_ngram) == 0) {
                ngrams[j]->count++;
                match_found = 1;
                break;
            }
        }
        if (!match_found) {
            ngrams[ngrams_count] = (ngram *) malloc(sizeof(ngram));
            ngrams[ngrams_count]->string = (wchar_t *) malloc(sizeof(wchar_t) * n);
            wcscpy(ngrams[ngrams_count]->string, current_ngram);
            ngrams[ngrams_count]->count = 1;
            ngrams_count++;
        }

        free(current_ngram);

    }

    int max_count = 0;
    int max_count_index = 0;

    for (int i = 0; i < ngrams_count; i++) {
        if (ngrams[i]->count > max_count) {
            max_count = ngrams[i]->count;
            max_count_index = i;
        }
    }

    wcscpy(result, ngrams[max_count_index]->string);
    for (int i = 0; i < ngrams_count; i++) {
        free(ngrams[i]->string);
        free(ngrams[i]);
    }
}

void apply_key(const wchar_t key[ALPHABET_SIZE][2], wchar_t *string, wchar_t *result) {
    unsigned int l = wcslen(string);
    for (int i = 0; i < l; i++) {
        if (!iswalpha(string[i])) {
            result[i] = string[i];
            continue;
        }

        result[i] = key[wchar_to_array_index(string[i])][1];
    }
}

void analysis_init() {
    state.string = readfile();
}

wchar_t *get_source_string() {
    return state.string;
}


