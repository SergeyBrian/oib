#include "analysis.h"

#define VARIABLE_LETTERS_COUNT 16

static analysis_state state;
static const wchar_t variable_symbols[VARIABLE_LETTERS_COUNT] = L"DFGIJLNQRSTUVWYZ";

void analysis_init() {
    int size = 100000;
    if (state.ready) return;

    state.bigrams_frequencies = (double *) calloc(sizeof(double), MAX_BIGRAMS);

    state.most_frequent_bigrams = (wchar_t **) malloc(sizeof(wchar_t *) * MAX_BIGRAMS);
    for (int i = 0; i < MAX_BIGRAMS; i++) {
        state.most_frequent_bigrams[i] = (wchar_t *) calloc(sizeof(wchar_t), 2);
    }

    state.decoded_bigrams = (wchar_t **) malloc(sizeof(wchar_t *) * MAX_BIGRAMS);
    for (int i = 0; i < MAX_BIGRAMS; i++) {
        state.decoded_bigrams[i] = (wchar_t *) calloc(sizeof(wchar_t), 2);
    }

    state.words = (wchar_t **) malloc(sizeof(wchar_t *) * MAX_WORDS);
    for (int i = 0; i < MAX_WORDS; i++) {
        state.words[i] = (wchar_t *) calloc(sizeof(wchar_t), MAX_WORD_LENGTH);
    }

    state.decoded_words = (wchar_t **) malloc(sizeof(wchar_t *) * MAX_WORDS);
    for (int i = 0; i < MAX_WORDS; i++) {
        state.decoded_words[i] = (wchar_t *) calloc(sizeof(wchar_t), MAX_WORD_LENGTH);
    }

    state.decoded_words_sorted = (wchar_t **) malloc(sizeof(wchar_t *) * MAX_WORDS);
    for (int i = 0; i < MAX_WORDS; i++) {
        state.decoded_words_sorted[i] = (wchar_t *) calloc(sizeof(wchar_t), MAX_WORD_LENGTH);
    }

    state.ready = 1;

    wchar_t *result = (wchar_t *) malloc(sizeof(wchar_t) * size);

    state.string = readfile();
    state.decoded_string = result;
    state.words_count = 0;
    state.min_valid_words = DEFAULT_MIN_VALID_WORDS;
    state.history_index = -1;

    read_wordlist();


    measure_letters_frequency();
    double *frequencies = get_frequencies();
    match_frequencies(frequencies, FREQUENCIES_RU, state.key);
    add_key_to_history();


    sort_words_by_length(state.string, state.words);
    apply_key();
}

void measure_letters_frequency() {
    unsigned int l = wcslen(state.string);
    unsigned int letters_count = 0;
    wchar_t bigram[3] = L"";
    int bigrams_count = 0;

    for (int i = 0; i < l; i++) {
        if (!iswalpha(state.string[i])) {
            bigram[0] = L'\0';
            bigram[1] = L'\0';
            bigram[2] = L'\0';
            continue;
        }

        int bigram_len = wcslen(bigram);
        if (bigram_len == 2) {
            int bigram_index = wcs_index(state.most_frequent_bigrams, MAX_BIGRAMS, bigram);
            if (bigram_index == -1) {
                wcscpy(state.most_frequent_bigrams[bigrams_count], bigram);
                state.bigrams_frequencies[bigrams_count++] = 1;
            } else {
                state.bigrams_frequencies[bigram_index]++;
            }
            bigram[0] = bigram[1];
            bigram[1] = state.string[i];
        } else {
            bigram[wcslen(bigram)] = state.string[i];
        }

        state.frequencies[wchar_to_array_index(towlower(state.string[i]))]++;
        letters_count++;
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        state.frequencies[i] /= letters_count;
    }

    for (int i = 0; i < bigrams_count; i++) {
        state.bigrams_frequencies[i] /= letters_count;
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

    for (int i = 0; i < get_words_count(); i++) {
        apply_key_to_str(get_words()[i], get_decoded_words()[i]);
    }

    return state.decoded_string;
}

int *get_key_ptr() {
    return state.key;
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

match_type does_match_mask(const wchar_t *string, const wchar_t *mask) {
    unsigned int l = wcslen(string);
    wchar_t variables[10] = L"";
    match_type result = STRICT_MATCH;

    if (l != wcslen(mask)) return NO_MATCH;
    for (int i = 0; i < l; i++) {
        if (iswlower(mask[i])) {
            if (string[i] != mask[i]) return NO_MATCH;
            continue;
        }
        if (INDEX_OF(mask, l, string[i]) != -1) result = MATCH;
        if (!variables[wchar_index(variable_symbols, mask[i])]) {
            variables[wchar_index(variable_symbols, mask[i])] = string[i];
            int c = 0;
            for (int j = 0; j < 10; j++) {
                if (variables[j] == string[i]) {
                    c++;
                    if (c > 1) return NO_MATCH;
                }
            }
            continue;
        }
        if (variables[INDEX_OF(variable_symbols, VARIABLE_LETTERS_COUNT, mask[i])] != string[i]) return NO_MATCH;
    }
    return result;
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
    return state.decoded_words;
}

wchar_t **get_words_sorted_by_decoded_letters() {
    sort_words_by_decoded_letters(state.decoded_string, state.decoded_words_sorted);
    return state.decoded_words_sorted;
}


int get_words_count() {
    return state.words_count;
}

void set_words_count(int count) {
    state.words_count = count;
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

double *get_bigrams_frequencies() {
    return state.bigrams_frequencies;
}

wchar_t **get_bigrams() {
    return state.most_frequent_bigrams;
}

wchar_t **get_decoded_bigrams() {
    for (int i = 0; i < MAX_BIGRAMS; i++) {
        apply_key_to_str(state.most_frequent_bigrams[i], state.decoded_bigrams[i]);
    }
    return state.decoded_bigrams;
}

void set_wordlist(wchar_t **wordlist, int len) {
    state.wordlist = wordlist;
    state.wordlist_length = len;
}

void auto_generate_key(void(*callback)(double f, int m)) {
    wchar_t **words = state.decoded_words;
    int *used_matches = calloc(sizeof(int), state.wordlist_length);
    int l = state.words_count;
    int passes = 0;

    // Iterate over words from longest to shortest
    // Find matching word from wordlist for every word
    // When all letters all assign, validate the key
    // If key not valid, reset all and repeat process, but choose new matching words
    do {
        passes++;
        add_key_to_history();
        for (int i = 0; i < ALPHABET_SIZE; i++) state.key[i] = -1;
        for (int i = l-1; i >= 0; i--) {
            callback((double)(l-i)/l, passes);
            wchar_t mask[MAX_WORD_LENGTH] = L"";
            generate_mask(words[i], mask);
            for (int j = 0; j < state.wordlist_length; j++) {
                if (used_matches[j]) continue;
                if (does_match_mask(state.wordlist[j], mask) != STRICT_MATCH) continue;
                generate_key_from_matches(state.words[i], state.wordlist[j]);
                apply_key();
                used_matches[j] = 1;
                break;
            }
        }
    } while (!is_key_valid());
}
