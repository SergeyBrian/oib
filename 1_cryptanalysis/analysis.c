#include "analysis.h"

static analysis_state state;
static const wchar_t variable_symbols[10] = L"_@#$%^&*+=";

void measure_letters_frequency() {
    unsigned int l = wcslen(state.string);
    unsigned int letters_count = 0;

    for (int i = 0; i < l; i++) {
        if (!iswalpha(state.string[i])) {
            continue;
        }

        state.frequencies[wchar_to_array_index(towlower(state.string[i]))]++;
        letters_count++;
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        state.frequencies[i] /= letters_count;
    }
}

void find_most_frequent_ngram(int n, wchar_t *result) {
    unsigned int l = wcslen(state.string);
    ngram *ngrams[1000] = {0};
    int ngrams_count = 0;

    for (int i = 0; i < l - n; i++) {
        wchar_t *current_ngram = (wchar_t *) malloc(sizeof(wchar_t) * (n + 1));
        for (int j = 0; j < n; j++) {
            if (state.string[i + j] == L' ') continue;
            current_ngram[j] = state.string[i + j];
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
        int a = state.key[wchar_to_array_index(state.string[i])];
        wchar_t c = LOWERCASE_ALPHABET_RU[a];
        state.decoded_string[i] = c;
    }


    return state.decoded_string;
}

int *get_key_ptr() {
    return state.key;
}

void analysis_init() {
    int size = 100000;
    if (state.ready) return;

    state.ready = 1;

    wchar_t *result = (wchar_t *) malloc(sizeof(wchar_t) * size);

    state.string = readfile();
    state.decoded_string = result;
    state.words_count = 0;
    state.min_valid_words = DEFAULT_MIN_VALID_WORDS;
    state.history_index = -1;


    measure_letters_frequency();
    double *frequencies = get_frequencies();
    match_frequencies(frequencies, FREQUENCIES_RU, state.key);
    add_key_to_history();

    state.words = (wchar_t **) malloc(sizeof(wchar_t *) * MAX_WORDS);
    for (int i = 0; i < MAX_WORDS; i++) {
        state.words[i] = (wchar_t *) calloc(sizeof(wchar_t), MAX_WORD_LENGTH);
    }

    state.decoded_words = (wchar_t **) malloc(sizeof(wchar_t *) * MAX_WORDS);
    for (int i = 0; i < MAX_WORDS; i++) {
        state.decoded_words[i] = (wchar_t *) calloc(sizeof(wchar_t), MAX_WORD_LENGTH);
    }

    sort_words_by_length(state.string, state.words, NULL);
    for (int i = 0; state.words[i] != NULL; i++) {
        state.words_count++;
    }
}

wchar_t *get_source_string() {
    return state.string;
}

wchar_t *get_decoded_string() {
    return state.decoded_string;
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

void generate_mask(wchar_t *string, wchar_t *mask) {
    unsigned int l = wcslen(string);
    wchar_t variables[ALPHABET_SIZE] = L"";

    int variables_count = 0;

    for (int i = 0; i < l; i++) {
        if (iswlower(string[i])) {
            mask[i] = string[i];
            continue;
        }
        if (!variables[wchar_to_array_index(string[i])]) {
            variables[wchar_to_array_index(string[i])] = variable_symbols[variables_count++];
        }
        mask[i] = variables[wchar_to_array_index(string[i])];
    }
}

int does_match_mask(const wchar_t *string, const wchar_t *mask, int strict_mode) {
    unsigned int l = wcslen(string);
    wchar_t variables[10] = L"";

    if (l != wcslen(mask)) return 0;
    for (int i = 0; i < l; i++) {
        if (iswalpha(mask[i])) {
            if (string[i] != mask[i]) return 0;
            continue;
        }
        if (strict_mode && DOES_CONTAIN(mask, l, string[i])) return 0;
        if (!variables[wchar_index(variable_symbols, mask[i])]) {
            variables[wchar_index(variable_symbols, mask[i])] = string[i];
            int c = 0;
            for (int j = 0; j < 10; j++) {
                if (variables[j] == string[i]) {
                    c++;
                    if (c > 1) return 0;
                }
            }
            continue;
        }
        if (variables[wchar_index(variable_symbols, mask[i])] != string[i]) return 0;
    }
    return 1;
}

void apply_key_to_str(const wchar_t *string, wchar_t *decoded_string) {
    unsigned int l = wcslen(string);
    for (int i = 0; i < l; i++) {
        if (!iswalpha(string[i]) || state.key[wchar_to_array_index(string[i])] == -1) {
            decoded_string[i] = string[i];
            continue;
        }
        decoded_string[i] = LOWERCASE_ALPHABET_RU[state.key[wchar_to_array_index(string[i])]];
    }
}

wchar_t **get_words() {
    return state.words;
}

wchar_t **get_decoded_words() {
    sort_words_by_length(state.decoded_string, state.decoded_words, NULL);
    return state.decoded_words;
}

int get_words_count() {
    return state.words_count;
}

void generate_key_from_matches(const wchar_t *encoded, const wchar_t *decoded) {
    unsigned int l = wcslen(encoded);
    for (int i = 0; i < l; i++) {
        int a = wchar_to_array_index(towlower(encoded[i]));
        int b = wchar_to_array_index(decoded[i]);
        state.key[a] = b;
    }
}

int evaluate_word(const wchar_t *word) {
    const wchar_t *p = word;

    unsigned int l = wcslen(word);

    if (l >= 2) {
        int vowels_count = 0;

        for (int i = 0; i < l; i++) {
            if (is_vowel(word[i])) {
                if (vowels_count >= 2) return 0;
                vowels_count++;
            } else if (vowels_count) break;
        }
        if (!vowels_count) return 0;
    }


    while (iswalpha(*(p + 1))) {
        for (int i = 0; i < IMPOSSIBLE_COMBINATIONS_LIST_LENGTH; i++) {
            if ((IMPOSSIBLE_COMBINATIONS_RU[i][0] == p[0] && IMPOSSIBLE_COMBINATIONS_RU[i][1] == p[1]) ||
                (IMPOSSIBLE_COMBINATIONS_RU[i][0] == p[1] && IMPOSSIBLE_COMBINATIONS_RU[i][1] == p[0])) {
                return 0;
            }
        }
        p++;
    }

    return 1;
}

int is_key_valid() {
    apply_key();
    wchar_t **words = get_decoded_words();

    int valid_words = 0;

    for (int i = 0; i < get_words_count(); i++) {
        valid_words += evaluate_word(words[i]);
    }

    double r = (double) valid_words / get_words_count();
    return (r >= state.min_valid_words);
}

void add_key_to_history() {
    for (int i = 0; state.history_index > -1 && i < ALPHABET_SIZE; i++) {
        if (state.key_history[state.history_index][i] != state.key[i]) break;
        if (i == ALPHABET_SIZE - 1) return;
    }
    state.history_index++;
    if (state.history_index == HISTORY_SIZE) {
        for (int i = 0; i < HISTORY_SIZE - COUNT_OF_HISTORY_KEYS_TO_REMOVE; i++) {
            free(state.key_history[i]);
            state.key_history[i] = state.key_history[i + COUNT_OF_HISTORY_KEYS_TO_REMOVE];
            state.key_history[i + COUNT_OF_HISTORY_KEYS_TO_REMOVE] = NULL;
        }
        state.history_index -= COUNT_OF_HISTORY_KEYS_TO_REMOVE;
    }

    if (state.key_history[state.history_index] == NULL)
        state.key_history[state.history_index] = calloc(sizeof(int), ALPHABET_SIZE);
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        state.key_history[state.history_index][i] = state.key[i];
    }

}

void undo_key_change() {
    add_key_to_history();
    if ((state.history_index - 1) < 0) return;
    state.history_index--;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        state.key[i] = state.key_history[state.history_index][i];
    }
}

void redo_key_change() {
    if (state.key_history[state.history_index + 1] == NULL) return;
    state.history_index++;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        state.key[i] = state.key_history[state.history_index][i];
    }
}
