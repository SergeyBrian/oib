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
} ui_state;

void ui_init();
void ui_set_page(ui_page page);
void ui_update();
void ui_quit();

#endif //INC_1_CRYPTANALYSIS_UI_H
