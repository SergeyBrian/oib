#include "ui.h"

static ui_state state;

void ui_init() {
    initscr();
    echo();
    halfdelay(1);
    state.height = LINES;
    state.width = COLS;
}

void ui_set_page(ui_page page) {
    state.current_page = page;
}

int show_file_open_error = 0;

void file_selector() {
    while (!state.is_input_file_open) {
        WINDOW *local_win;
        local_win = newwin(LINES / 2, COLS / 2, LINES / 4, COLS / 4);
        box(local_win, 0, 0);

        if (show_file_open_error) {
            mvwprintw(local_win, 0, 2, "Can't open file!");
        }


        mvwprintw(local_win, 1, 2, "Enter input file name: ");
        wrefresh(local_win);
        char filename[1000];
        wscanw(local_win, "%s", filename);

        if (open_file(filename)) {
            state.current_page = MAIN_PAGE;
            show_file_open_error = 0;
            state.is_input_file_open = 1;
            return;
        }

        show_file_open_error = 1;
    }
}

void draw_frequencies_tab() {
    int width = (COLS / 3 - 2)/2;
    int height = LINES - 3;
    WINDOW *frequencies_tab = newwin(height, width, 1, 2 * COLS / 3 + 1);
    WINDOW *expected_frequencies_tab = newwin(height, width, 1, 2 * COLS/ 3 +  (COLS / 3 - 2)/2 + 1);
    double *frequencies = get_frequencies();

    int indexes[ALPHABET_SIZE];
    int expected_indexes[ALPHABET_SIZE];
    int matches[ALPHABET_SIZE];
    sort_indexes(frequencies, indexes);
    sort_indexes(FREQUENCIES_RU, expected_indexes);
    match_frequencies(frequencies, FREQUENCIES_RU, matches);

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        mvwprintw(frequencies_tab, 1 + i, 2, "%C = %lf (%C)\n", ALPHABET_RU[indexes[i]], frequencies[indexes[i]], ALPHABET_RU[matches[indexes[i]]]);
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        mvwprintw(expected_frequencies_tab, 1 + i, 2, "%C = %lf\n", ALPHABET_RU[expected_indexes[i]], FREQUENCIES_RU[expected_indexes[i]]);
    }

    box(frequencies_tab, 0, 0);
    box(expected_frequencies_tab, 0, 0);
    mvwprintw(frequencies_tab, 0, 1, "Frequencies table");
    mvwprintw(expected_frequencies_tab, 0, 1, "Expected frequencies");
    wrefresh(frequencies_tab);
    wrefresh(expected_frequencies_tab);
}

void main_page() {
    noecho();
    clear();
    refresh();
    int width = 2 * COLS / 3;
    WINDOW *top_window = newwin(LINES / 2, width, 1, 1);
    box(top_window, 0, 0);
    analysis_init();
    measure_letters_frequency();
    do {
        wchar_t *source_string = get_source_string();
        unsigned int l = wcslen(source_string);
        for (int i = 0; (i * (width - 5)) < l; i++)
        mvwaddnwstr(top_window, 1+i, 2, source_string+i*(width-5), width - 5);
        wrefresh(top_window);
        draw_frequencies_tab();
        refresh();
    } while (getch() != 'q');
    echo();
    quit();
}

void render_page(ui_page page) {
    switch (page) {
        case FILE_SELECTOR:
            file_selector();
            break;
        case MAIN_PAGE:
            main_page();
            break;
        default:
            break;
    };
}

void ui_update() {
    clear();
    render_page(state.current_page);
    refresh();
}

void ui_quit() {
    endwin();
}
