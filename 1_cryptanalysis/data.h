#ifndef INC_1_CRYPTANALYSIS_DATA_H
#define INC_1_CRYPTANALYSIS_DATA_H

#include <wchar.h>

#define ALPHABET_SIZE 32
#define FIRST_ALPHABET_LETTER_WCHAR_VALUE 1072
#define MIN_OFFSET_MATCH_COUNT 10

extern const wchar_t *MOST_FREQUENT_TRIGRAM_RU;
extern const wchar_t *MOST_FREQUENT_BIGRAM_RU;
extern const wchar_t *MOST_FREQUENT_LETTER_RU;

extern const wchar_t *ALPHABET_RU;

// {Average frequency, Frequency at word start, frequency at word middle, frequency at word end}
extern const double FREQUENCIES_RU[][4];

#endif //INC_1_CRYPTANALYSIS_DATA_H