#ifndef INC_1_CRYPTANALYSIS_UI_H
#define INC_1_CRYPTANALYSIS_UI_H

//#include <ncurses.h>
#include <ncursesw/curses.h>

#include "fs.h"
#include "analysis.h"

typedef enum {
    FILE_SELECTOR,
    MAIN_PAGE
} ui_page;


typedef struct {
    ui_page current_page;
    int active_letter;
    int is_input_file_open;
    int height;
    int width;
    int show_decoded;
    int indexes[ALPHABET_SIZE];
    int expected_indexes[ALPHABET_SIZE];
} ui_state;

void ui_init();
void ui_set_page(ui_page page);
void ui_update();
void ui_quit();

#endif //INC_1_CRYPTANALYSIS_UI_H
