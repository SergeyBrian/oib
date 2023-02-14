#include "utils.h"


void error_exit(const char *message) {
    printf("%s", message);
    exit(1);
}

int most_frequent_value(const int *arr, int size) {
    int i, j, max_count = 0, res = arr[0];
    for (i = 0; i < size; i++) {
        int count = 0;
        for (j = 0; j < size; j++) {
            if (arr[i] == arr[j]) {
                count++;
            }
        }
        if (count > max_count) {
            max_count = count;
            res = arr[i];
        }
    }
    return res;
}

void quit() {
    ui_quit();
    exit(0);
}

int wchar_to_array_index(wchar_t c) {
    return (int) (c) - FIRST_ALPHABET_LETTER_WCHAR_VALUE;
}

int absolute_index(int i, int array_size) {
    i = i % array_size;
    if (i < 0) {
        i = array_size - i - 2;
    }
    return i;
}

void sort_indexes(const double arr[], int *indexes) {

    double temp;
    int tempIndex;


    // Initialize the indexes array
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        indexes[i] = i;
    }

    // Sort the indexes based on the elements of the array
    for (int i = 0; i < ALPHABET_SIZE - 1; i++) {
        for (int j = i + 1; j < ALPHABET_SIZE; j++) {
            if (arr[indexes[j]] > arr[indexes[i]]) {
                tempIndex = indexes[i];
                indexes[i] = indexes[j];
                indexes[j] = tempIndex;
            }
        }
    }
}


int count_decoded_letters(wchar_t *word) {
    unsigned int l = wcslen(word);
    int count = 0;
    for (int i = 0; i < l; i++) {
        if (iswlower(word[i])) count++;
    }

    return count;
}

void
sort_words_by_function(const wchar_t *input_str, wchar_t *words[], wchar_t *to_free[], unsigned long (*func)(wchar_t *),
                       int desc) {
    wchar_t *str_copy = wcsdup(input_str);
    wchar_t *p = str_copy;
    int word_count = 0;

    unsigned int l = wcslen(str_copy);
    for (int i = 0; i < l; i++) {
        if (iswalpha(str_copy[i])) continue;
        str_copy[i] = L' ';
    }

    while (*p != L'\0' && word_count < MAX_WORDS) {
        while (iswspace(*p)) {
            ++p;
        }
        if (*p == L'\0') {
            break;
        }
        wchar_t *start = p;
        while (*p != L'\0' && !iswspace(*p)) {
            ++p;
        }
        *p = L'\0';
        ++p;
        words[word_count] = (wchar_t *) malloc(sizeof(wchar_t) * (wcslen(start) + 1));
        if (to_free != NULL) to_free[word_count] = words[word_count];
        wcscpy(words[word_count++], start);
    }

    for (int i = 0; i < word_count - 1; ++i) {
        for (int j = i + 1; j < word_count; ++j) {
            if ((desc) ? func(words[i]) < func(words[j]) : func(words[i]) > func(words[j])) {
                wchar_t *temp = words[i];
                words[i] = words[j];
                words[j] = temp;
            }
        }
    }

    int unique_count = 0;
    for (int i = 0; i < word_count; ++i) {
        int is_duplicate = 0;
        for (int j = 0; j < unique_count; ++j) {
            if (wcscmp(words[i], words[j]) == 0) {
                is_duplicate = 1;
                break;
            }
        }
        if (!is_duplicate) {
            words[unique_count++] = words[i];
        }
    }


    words[unique_count] = NULL;

    free(str_copy);
}

void sort_words_by_length(const wchar_t *input_str, wchar_t *words[], wchar_t *to_free[]) {
    sort_words_by_function(input_str, words, to_free, (unsigned long (*)(wchar_t *)) wcslen, false);
}

void sort_words_by_decoded_letters(const wchar_t *input_str, wchar_t *words[], wchar_t *to_free[]) {
    sort_words_by_function(input_str, words, to_free, (unsigned long (*)(wchar_t *)) count_decoded_letters, true);
}

int count_word_occurrences(const wchar_t *input_str, const wchar_t *word) {
    int count = 0;
    const size_t word_len = wcslen(word);
    for (const wchar_t *p = input_str; *p != L'\0'; p++) {
        if (iswspace(*p)) {
            continue;
        }
        if (towlower(*p) == towlower(*word)) {
            const wchar_t *q = p + 1;
            size_t i = 1;
            while (i < word_len && *q != L'\0' && !iswspace(*q) && towlower(*q) == towlower(word[i])) {
                i++;
                q++;
            }
            if (i == word_len && (*q == L'\0' || iswspace(*q))) {
                count++;
            }
        }
    }
    return count;
}

int wchar_index(const wchar_t *str, wchar_t ch) {
    size_t len = wcslen(str);
    size_t i;

    for (i = 0; i < len; i++) {
        if (str[i] == ch) {
            return (int) i;
        }
    }

    return -1;
}

int char_index(const char *str, char ch) {
    char *ptr = strchr(str, ch);

    if (ptr != NULL) {
        return (int) (ptr - str);
    } else {
        return -1;
    }
}

