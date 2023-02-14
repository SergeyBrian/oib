#include "ui.h"

static ui_state state;
static const wchar_t letters_controls_text[] = L"[Tab] Режим просмотра текста  [↑]/[↓] Выбор буквы  "
                                               "[←]/[→] Изменить выбранную букву  [BACKSPACE] Сбросить выбранную букву  "
                                               "[W] Режим просмотра слов";
static const wchar_t common_controls_text[] = L"[Q] Выйти";

void ui_init() {
    initscr();
    echo();
    state.height = LINES;
    state.width = COLS;
    state.show_decoded = 0;
    state.word_view_mode = VIEW_BY_LETTERS_COUNT;
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

void draw_controls_tab() {
    int width = COLS - 3;
    int height = 4;
    WINDOW *controls = newwin(height, width, LINES - 5, 1);
    box(controls, 0, 0);
    mvwprintw(controls, 0, 1, "Управление");
    mvwprintw(controls, 2, 1, "%S  %S", letters_controls_text, common_controls_text);
    wrefresh(controls);
}

void draw_words_by_letters_count(WINDOW *words_tab) {
    wchar_t **words = sort_words_by_length((state.show_decoded) ? get_decoded_string() : get_source_string());
    unsigned int x = 2;
    unsigned int y = 2;
    for (int i = 0; words[i] != NULL; i++) {
        mvwprintw(words_tab, y, x, "%S ", words[i]);
        x += wcslen(words[i]) + 1;
        if (x + ((words[i + 1]) ? wcslen(words[i + 1]) : 0) >= (2 * COLS / 3) - 5) {
            y++;
            x = 2;
        }
    }
    wrefresh(words_tab);
}

void draw_words_tab() {
    int width = 2 * COLS / 3;
    int height = (LINES / 2) - 5;
    WINDOW *words_tab = newwin(height, width, 1 + LINES / 2, 1);
    box(words_tab, 0, 0);
    mvwprintw(words_tab, 0, 1, "Слова %S %S", (state.word_view_mode == VIEW_BY_LETTERS_COUNT) ? L"по количеству букв"
                                                                                              : L"по количеству расшифрованных букв",
              (state.show_decoded) ? L"(Расшифрованные)" : L"(Исходные)");
    wrefresh(words_tab);
    switch (state.word_view_mode) {
        case VIEW_BY_LETTERS_COUNT:
            draw_words_by_letters_count(words_tab);
            break;
        case VIEW_BY_DECODED_LETTERS_COUNT:
            break;
    }
}

void draw_frequencies_tab() {
    int width = (COLS / 3 - 2) / 2;
    int height = LINES - 6;
    WINDOW *frequencies_tab = newwin(height, width, 1, 2 * COLS / 3 + 1);
    WINDOW *expected_frequencies_tab = newwin(height, width, 1, 2 * COLS / 3 + (COLS / 3 - 2) / 2 + 1);
    double *frequencies = get_frequencies();

    int *key = get_key_ptr();

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        mvwprintw(frequencies_tab, 1 + i, 2, "%C = %lf", ALPHABET_RU[state.indexes[i]], frequencies[state.indexes[i]]);
        wchar_t suggested_letter = (key[state.indexes[i]] > -1) ? ALPHABET_RU[key[state.indexes[i]]] : L'?';
        if (i == state.active_letter)
            mvwprintw(frequencies_tab, 1 + i, 14, " [%C]\n", suggested_letter);
        else
            mvwprintw(frequencies_tab, 1 + i, 14, "  %C\n", suggested_letter);
    }


    for (int i = 0; i < ALPHABET_SIZE; i++) {
        mvwprintw(expected_frequencies_tab, 1 + i, 2, "%C = %lf\n", ALPHABET_RU[state.expected_indexes[i]],
                  FREQUENCIES_RU[state.expected_indexes[i]]);
    }

    box(frequencies_tab, 0, 0);
    box(expected_frequencies_tab, 0, 0);
    mvwprintw(frequencies_tab, 0, 1, "Реальные частоты");
    mvwprintw(expected_frequencies_tab, 0, 1, "Ожидаемые частоты");
    wrefresh(frequencies_tab);
    wrefresh(expected_frequencies_tab);
}

void main_page() {
    noecho();
    keypad(stdscr, true);
    int width = 2 * COLS / 3;
    WINDOW *top_window = newwin(LINES / 2, width, 1, 1);
    analysis_init();
    sort_indexes(get_frequencies(), state.indexes);
    sort_indexes(FREQUENCIES_RU, state.expected_indexes);
    wchar_t ch;
    int *key = get_key_ptr();
    do {
        wclear(top_window);
        refresh();
        if (ch) {
            switch (ch) {
                case KEY_DOWN:
                case 'j':
                    state.active_letter = absolute_index(state.active_letter + 1, ALPHABET_SIZE);
                    break;
                case KEY_UP:
                case 'k':
                    state.active_letter = absolute_index(state.active_letter - 1, ALPHABET_SIZE);
                    break;
                case '\t':
                    state.show_decoded = !state.show_decoded;
                    break;
                case KEY_RIGHT:
                case 'l':
                    key[state.indexes[state.active_letter]] = (key[state.indexes[state.active_letter]] == -1)
                                                              ? state.indexes[state.active_letter] : absolute_index(
                                    key[state.indexes[state.active_letter]] + 1, ALPHABET_SIZE);
                    break;
                case KEY_LEFT:
                case 'h':
                    key[state.indexes[state.active_letter]] = (key[state.indexes[state.active_letter]] == -1)
                                                              ? state.indexes[state.active_letter] : absolute_index(
                                    key[state.indexes[state.active_letter]] - 1, ALPHABET_SIZE);
                    break;
                case KEY_BACKSPACE:
                    key[state.indexes[state.active_letter]] = -1;
                    break;
                case 'w':
                    state.word_view_mode = !state.word_view_mode;
                    break;
            }
        }
        wchar_t *string = (state.show_decoded) ? apply_key() : get_source_string();
        unsigned int l = wcslen(string);
        for (int i = 0; (i * (width - 5)) < l; i++)
            mvwaddnwstr(top_window, 1 + i, 2, string + i * (width - 5), width - 5);

        box(top_window, 0, 0);
        mvwprintw(top_window, 0, 1, (state.show_decoded) ? "Расшифрованный текст" : "Исходный текст");
        wrefresh(top_window);
        draw_frequencies_tab();
        draw_words_tab();
        draw_controls_tab();
        refresh();
    } while ((ch = getch()) != 'q');
    echo();
    keypad(stdscr, false);
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
