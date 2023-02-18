#ifndef INC_1_CRYPTANALYSIS_UI_H
#define INC_1_CRYPTANALYSIS_UI_H

//#include <ncurses.h>
#include <ncursesw/curses.h>

#include "fs.h"
#include "analysis.h"

typedef enum {
    FILE_SELECTOR,
    MAIN_PAGE,
    UI_PAGE_ENUM_SIZE
} ui_page;

typedef enum {
    VIEW_BY_LETTERS_COUNT,
    VIEW_BY_DECODED_LETTERS_COUNT,
    WORD_ANALYSIS,
    UI_WORD_VIEW_MODE_ENUM_SIZE
} ui_word_view_mode;

typedef enum {
    SELECT_WORD,
    ANALYSE_WORD,
    UI_WORD_ANALYSIS_MODE_ENUM_SIZE
} ui_word_analysis_mode;


typedef struct {
    ui_page current_page;
    int active_letter;
    int is_input_file_open;
    int height;
    int width;
    int show_decoded;
    int indexes[ALPHABET_SIZE];
    int expected_indexes[ALPHABET_SIZE];
    ui_word_view_mode word_view_mode;
    ui_word_analysis_mode word_analysis_mode;
    int word_to_analyse_index;
    wchar_t *word_to_analyse;
    int skip_input;
    int matching_word_index;
    wchar_t custom_match_words[MAX_WORD_LENGTH][MAX_WORDS];
    int custom_match_words_count;
    int bigrams_indexes[MAX_BIGRAMS];
    int bigrams_expected_indexes[MAX_BIGRAMS];
    int is_wordlist_file_open;
} ui_state;

void ui_init();

void ui_set_page(ui_page page);

void ui_set_wordlist_open();

void ui_update();

void ui_quit();

#endif //INC_1_CRYPTANALYSIS_UI_H
