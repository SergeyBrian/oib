#include "ui.h"

ui_state state;

void ui_init() {
    initscr();
    echo();
    halfdelay(10);
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
        local_win = newwin(LINES/2, COLS/2, LINES/4, COLS/4);
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

void main_page() {
    WINDOW *top_window = newwin(LINES/2, COLS - 2, 1, 1);
    box(top_window, 0, 0);
    analysis_init();
    do {
        wchar_t *source_string = get_source_string();
        mvwaddwstr(top_window, 2, 2, source_string);
    } while (getch() != 'q');
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
