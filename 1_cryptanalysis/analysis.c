#include "analysis.h"

static analysis_state state;

void measure_letters_frequency() {
    unsigned int l = wcslen(state.string);
    unsigned int letters_count = 0;

    for (int i = 0; i < l; i++) {
        if (!iswalpha(state.string[i])) {
            continue;
        }

        state.frequencies[wchar_to_array_index(state.string[i])]++;
        letters_count++;
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        state.frequencies[i] /= letters_count;
    }
}

void find_most_frequent_ngram(const wchar_t *string, int n, wchar_t *result) {
    unsigned int l = wcslen(string);
    ngram *ngrams[1000] = {0};
    int ngrams_count = 0;

    for (int i = 0; i < l - n; i++) {
        wchar_t *current_ngram = (wchar_t *) malloc(sizeof(wchar_t) * (n + 1));
        for (int j = 0; j < n; j++) {
            if (string[i + j] == L' ') continue;
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

wchar_t *apply_key() {
    unsigned int l = wcslen(state.string);
    for (int i = 0; i < l; i++) {
        if (!iswalpha(state.string[i]) || state.key[wchar_to_array_index(state.string[i])] == -1) {
            state.decoded_string[i] = state.string[i];
            continue;
        }
        state.decoded_string[i] = LOWERCASE_ALPHABET_RU[state.key[wchar_to_array_index(state.string[i])]];
    }


    return state.decoded_string;
}

int *get_key_ptr() {
    return state.key;
}

void analysis_init() {
    int size = 100000;

    wchar_t *result = (wchar_t *) malloc(sizeof(wchar_t) * size);

    state.string = readfile();
    state.decoded_string = result;


    measure_letters_frequency();
    double *frequencies = get_frequencies();
    match_frequencies(frequencies, FREQUENCIES_RU, state.key);
}

wchar_t *get_source_string() {
    return state.string;
}

double *get_frequencies() {
    return state.frequencies;
}

void match_frequencies(const double freq1[], const double freq2[], int matches[]) {

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        double min_diff = 100;
        int min_diff_index = -1;

        if (freq1[i] >= MIN_FREQUENCY)
            for (int j = 0; j < ALPHABET_SIZE; j++) {
                double diff = fabs(freq1[i] - freq2[j]);
                if (diff > MAX_FREQUENCY_DIFFERENCE) continue;
                if (diff < min_diff) {
                    min_diff = diff;
                    min_diff_index = j;
                }
            }
        int index_already_used = 0;
        for (int j = 0; min_diff_index != -1 && j < ALPHABET_SIZE; j++) {
            if (matches[j] == min_diff_index) {
                index_already_used = 1;
                break;
            }
        }
        matches[i] = (index_already_used) ? -1 : min_diff_index;
    }
}

