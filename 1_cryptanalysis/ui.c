#include "ui.h"

static ui_state state;
static const wchar_t letters_controls_text[] = L"[Tab] Режим просмотра текста  [↑]/[↓] Выбор буквы  "
                                               "[←]/[→] Изменить выбранную букву  [BACKSPACE] Сбросить выбранную букву";
static const wchar_t select_word_controls_text[] = L"[←]/[→] Выбор слова   [ENTER] Подтвердить выбор";
static const wchar_t analyse_word_controls_text[] = L"[BACKSPACE] Выбрать другое слово   [←]/[→] Выбор слова   [ENTER] Подтвердить выбор";
static const wchar_t common_controls_text[] = L"[W] Режим просмотра слов   [Q] Выйти";

#define TEXT_TAB_WIDTH (2 * COLS / 3)

#define WORDS_TAB_HEIGHT ((LINES / 2) - 5)
#define WORDS_TAB_WIDTH (2 * COLS / 3)

#define FREQUENCIES_TAB_WIDTH ((COLS / 3 - 2) / 2)
#define FREQUENCIES_TAB_HEIGHT (LINES - 6)

void draw_frequencies_tab();

void draw_words_tab_frame();

WINDOW *frequencies_tab;
WINDOW *expected_frequencies_tab;
WINDOW *words_tab;
WINDOW *text_tab;
WINDOW *controls_tab;
WINDOW *file_window;

void ui_init() {
    initscr();
    echo();
    state.height = LINES;
    state.width = COLS;
    state.show_decoded = 0;
    state.word_view_mode = VIEW_BY_LETTERS_COUNT;
    state.word_to_analyse_index = 0;
    state.word_to_analyse = calloc(sizeof(wchar_t), MAX_WORD_LENGTH);
    state.word_analysis_mode = SELECT_WORD;
    state.skip_input = 0;
    state.matching_word_index = 0;

    text_tab = newwin(LINES / 2, TEXT_TAB_WIDTH, 1, 1);
    controls_tab = newwin(4, COLS - 3, LINES - 5, 1);
    words_tab = newwin(WORDS_TAB_HEIGHT, WORDS_TAB_WIDTH, 1 + LINES / 2, 1);
    frequencies_tab = newwin(FREQUENCIES_TAB_HEIGHT, FREQUENCIES_TAB_WIDTH, 1, 2 * COLS / 3 + 1);
    expected_frequencies_tab = newwin(FREQUENCIES_TAB_HEIGHT, FREQUENCIES_TAB_WIDTH, 1,
                                      2 * COLS / 3 + (COLS / 3 - 2) / 2 + 1);
}

void ui_set_page(ui_page page) {
    state.current_page = page;
}

int show_file_open_error = 0;

void file_selector() {
    while (!state.is_input_file_open) {
        file_window = newwin(LINES / 2, COLS / 2, LINES / 4, COLS / 4);
        box(file_window, 0, 0);

        if (show_file_open_error) {
            mvwprintw(file_window, 0, 2, "Can't open file!");
        }


        mvwprintw(file_window, 1, 2, "Enter input file name: ");
        wrefresh(file_window);
        char filename[1000];
        wscanw(file_window, "%s", filename);

        if (open_file(filename)) {
            state.current_page = MAIN_PAGE;
            show_file_open_error = 0;
            state.is_input_file_open = 1;
            return;
        }

        show_file_open_error = 1;
    }
    delwin(file_window);
}

void draw_controls_tab() {
    wclear(controls_tab);
    box(controls_tab, 0, 0);
    mvwprintw(controls_tab, 0, 1, "Управление");
    const wchar_t *controls_text;
    if (state.word_view_mode == WORD_ANALYSIS) {
        switch (state.word_analysis_mode) {
            case SELECT_WORD:
                controls_text = select_word_controls_text;
                break;
            case ANALYSE_WORD:
                controls_text = analyse_word_controls_text;
                break;
        }
    } else {
        controls_text = letters_controls_text;
    }
    mvwprintw(controls_tab, 2, 1, "%S  %S", controls_text, common_controls_text);
    wrefresh(controls_tab);
}

void draw_words_by_letters_count() {
    int x = 2;
    int y = 2;

    wchar_t **words = (state.show_decoded) ? get_decoded_words() : get_words();

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

void draw_words_by_decoded_letters_count() {
    wchar_t *words[MAX_WORDS] = {0};
    wchar_t *to_free[MAX_WORDS] = {0};
    sort_words_by_decoded_letters(apply_key(), words, to_free);

    int x = 2;
    int y = 2;
    for (int i = 0; words[i] != NULL; i++) {
        mvwprintw(words_tab, y, x, "%S ", words[i]);
        x += wcslen(words[i]) + 1;
        if (x + ((words[i + 1]) ? wcslen(words[i + 1]) : 0) >= (2 * COLS / 3) - 5) {
            y++;
            x = 2;
        }
    }
    for (int i = 0; i < MAX_WORDS; i++) {
        free(to_free[i]);
    }
    wrefresh(words_tab);
}

void draw_word_selector() {
    draw_words_tab_frame();
    mvwprintw(words_tab, 0, WORDS_TAB_WIDTH / 2 - 7, " [Выбор слова]");

    do {
        wchar_t **words = get_words();

        int x = 2;
        int y = 2;
        for (int i = 0; words[i] != NULL; i++) {
            char *format = (i == state.word_to_analyse_index) ? "[%S] " : " %S  ";
            mvwprintw(words_tab, y, x, format, words[i]);
            x += wcslen(words[i]) + 2;
            if (x + ((words[i + 1]) ? wcslen(words[i + 1]) : 0) >= (2 * COLS / 3) - 5) {
                y++;
                x = 2;
            }
        }
        wrefresh(words_tab);

        switch (getch()) {
            case 'w':
                state.word_view_mode = VIEW_BY_LETTERS_COUNT;
                state.skip_input = 1;
                break;
            case 'q':
                quit();
                break;
            case KEY_RIGHT:
            case 'l':
                state.word_to_analyse_index = absolute_index(state.word_to_analyse_index + 1, get_words_count());
                break;
            case KEY_LEFT:
            case 'h':
                state.word_to_analyse_index = absolute_index(state.word_to_analyse_index - 1, get_words_count());
                break;
            case '\n':
                state.word_analysis_mode = ANALYSE_WORD;
                wcscpy(state.word_to_analyse, words[state.word_to_analyse_index]);
                break;
        }
    } while (state.word_view_mode == WORD_ANALYSIS && state.word_analysis_mode == SELECT_WORD);
    state.skip_input = 1;
}

void select_word() {
    draw_controls_tab();
    state.active_letter = -1;
    draw_frequencies_tab();
    draw_word_selector();
}

void draw_analyse_word_tab() {
    draw_words_tab_frame();
    mvwprintw(words_tab, 0, WORDS_TAB_WIDTH/2 - (wcslen(state.word_to_analyse) + 2)/2, "[%S]", state.word_to_analyse);
    do {
        wchar_t decoded_word[MAX_WORD_LENGTH] = L"";
        apply_key_to_str(state.word_to_analyse, decoded_word);
        wchar_t mask[ALPHABET_SIZE] = L"";
        generate_mask(decoded_word, mask);
        mvwprintw(words_tab, 2, 2, "%S (%S)", decoded_word, mask);
        mvwhline(words_tab, WORDS_TAB_HEIGHT/4 - 1, 1, 0, WORDS_TAB_WIDTH-2);
        int x = 2;
        int y = WORDS_TAB_HEIGHT/4 + 1;
        int word_count = 0;

        for (int i = 0; i < WORDLIST_LENGTH; i++) {
            char *format = (i == state.matching_word_index) ? "[%S] " : " %S  ";
            if (!does_match_mask(FREQUENT_WORDS_RU[i], mask)) continue;
            word_count++;
            mvwprintw(words_tab, y, x, format, FREQUENT_WORDS_RU[i]);
            x += wcslen(FREQUENT_WORDS_RU[i]) + 2;
            if (x + ((FREQUENT_WORDS_RU[i + 1]) ? wcslen(FREQUENT_WORDS_RU[i + 1]) : 0) >= (2 * COLS / 3) - 5) {
                y++;
                x = 2;
            }
        }
        wrefresh(words_tab);
        refresh();
        switch (getch()) {
            case 'w':
                state.word_view_mode = VIEW_BY_LETTERS_COUNT;
                break;
            case 'q':
                quit();
                break;
            case KEY_BACKSPACE:
                state.word_analysis_mode = SELECT_WORD;
                break;
            case KEY_RIGHT:
            case 'l':
                state.matching_word_index = absolute_index(state.matching_word_index + 1, word_count);
                break;
            case KEY_LEFT:
            case 'h':
                state.matching_word_index = absolute_index(state.matching_word_index - 1, word_count);
                break;
        }
    } while (state.word_view_mode == WORD_ANALYSIS && state.word_analysis_mode == ANALYSE_WORD);
    state.skip_input = 1;
}

void analyse_word() {
    draw_controls_tab();
    draw_frequencies_tab();
    draw_analyse_word_tab();
}

void word_analysis() {
    do {
        switch (state.word_analysis_mode) {
            case SELECT_WORD:
                select_word();
                break;
            case ANALYSE_WORD:
                analyse_word();
                break;
        }
    } while (state.word_view_mode == WORD_ANALYSIS);
}

void draw_words_tab_frame() {
    wclear(words_tab);
    box(words_tab, 0, 0);
    wchar_t *tab_label;
    switch (state.word_view_mode) {
        case VIEW_BY_LETTERS_COUNT:
            tab_label = L"Слова по количеству букв";
            break;
        case VIEW_BY_DECODED_LETTERS_COUNT:
            tab_label = L"Слова по количеству расшифрованных букв";
            break;
        case WORD_ANALYSIS:
            tab_label = L"Анализ слова";
            break;
    }
    mvwprintw(words_tab, 0, 1, "%S%S", tab_label,
              (state.word_view_mode == VIEW_BY_LETTERS_COUNT) ? ((state.show_decoded) ? L" (Расшифрованные)"
                                                                                      : L" (Исходные)")
                                                              : L"");
}

void draw_words_tab() {
    draw_words_tab_frame();
    switch (state.word_view_mode) {
        case VIEW_BY_LETTERS_COUNT:
            draw_words_by_letters_count();
            break;
        case VIEW_BY_DECODED_LETTERS_COUNT:
            draw_words_by_decoded_letters_count();
            break;
        case WORD_ANALYSIS:
            word_analysis();
            break;
    }
    wrefresh(words_tab);
}

void draw_frequencies_tab() {
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

    box(frequencies_tab, '|', '_');
    box(expected_frequencies_tab, 0, 0);
    mvwprintw(frequencies_tab, 0, 1, "Реальные частоты");
    mvwprintw(expected_frequencies_tab, 0, 1, "Ожидаемые частоты");
    wrefresh(frequencies_tab);
    wrefresh(expected_frequencies_tab);
}

void main_page() {
    noecho();
    keypad(stdscr, true);
    analysis_init();
    sort_indexes(get_frequencies(), state.indexes);
    sort_indexes(FREQUENCIES_RU, state.expected_indexes);
    wchar_t ch;
    int *key = get_key_ptr();
    do {
        wclear(text_tab);
        refresh();
        if (!state.skip_input && ch) {
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
                    state.word_view_mode = absolute_index(state.word_view_mode + 1, UI_WORD_VIEW_MODE_ENUM_SIZE);
                    break;
            }
        }
        state.skip_input = 0;
        wchar_t *string = (state.show_decoded) ? apply_key() : get_source_string();
        unsigned int l = wcslen(string);
        for (int i = 0; (i * (TEXT_TAB_WIDTH - 5)) < l; i++)
            mvwaddnwstr(text_tab, 1 + i, 2, string + i * (TEXT_TAB_WIDTH - 5), TEXT_TAB_WIDTH - 5);

        box(text_tab, 0, 0);
        mvwprintw(text_tab, 0, 1, (state.show_decoded) ? "Расшифрованный текст" : "Исходный текст");
        wrefresh(text_tab);
        draw_frequencies_tab();
        draw_words_tab();
        draw_controls_tab();
        refresh();
    } while (state.skip_input || ((ch = getch()) != 'q'));
    delwin(text_tab);
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
    delwin(frequencies_tab);
    delwin(expected_frequencies_tab);
    delwin(words_tab);
    delwin(controls_tab);
    endwin();
}
