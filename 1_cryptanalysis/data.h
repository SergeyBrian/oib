/*
 * Frequent words list is based on info from http://dict.ruslang.ru/freq.php?act=show&dic=freq_freq&title=%D7%E0%F1%F2%EE%F2%ED%FB%E9%20%F1%EF%E8%F1%EE%EA%20%EB%E5%EC%EC
 * Letters frequencies are based on info from https://dpva.ru/Guide/GuideUnitsAlphabets/Alphabets/FrequencyRuLetters/
 * Extended letter frequencies are based on info from https://abakbot.ru/online-5/476-freq2
 */

#ifndef INC_1_CRYPTANALYSIS_DATA_H
#define INC_1_CRYPTANALYSIS_DATA_H

#include <wchar.h>

#define ALPHABET_SIZE 32
#define FIRST_ALPHABET_LETTER_WCHAR_VALUE 1040
#define MAX_FREQUENCY_DIFFERENCE 0.01
#define MIN_FREQUENCY 0.1
#define MAX_TEXT_LENGTH 100000
#define MAX_WORD_LENGTH 40

#define WORDLIST_LENGTH 90

extern const wchar_t *FREQUENT_WORDS_RU[WORDLIST_LENGTH];

extern const wchar_t *ALPHABET_RU;
extern const wchar_t *LOWERCASE_ALPHABET_RU;

extern const double FREQUENCIES_RU[];

// {Average frequency, Frequency at word start, frequency at word middle, frequency at word end}
extern const double EXTENDED_FREQUENCIES_RU[][4];

#endif //INC_1_CRYPTANALYSIS_DATA_H
