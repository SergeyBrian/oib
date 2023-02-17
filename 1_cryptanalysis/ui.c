#include "ui.h"

static ui_state state;
static const wchar_t letters_controls_text[] = L"[Tab] Режим просмотра текста  [↑]/[↓] Выбор буквы  "
                                               "[←]/[→] Изменить выбранную букву  [BACKSPACE] Сбросить выбранную букву";
static const wchar_t select_word_controls_text[] = L"[←]/[→] Выбор слова   [ENTER] Подтвердить выбор";
static const wchar_t analyse_word_controls_text[] = L"[BACKSPACE] Выбрать другое слово   [SPACE] Ввести свое слово   [←]/[→] Выбор слова   [ENTER] Подтвердить выбор";
static const wchar_t common_controls_text[] = L"[W] Режим просмотра слов   [Q] Выйти";
static wchar_t *key_validity_message;

#define TEXT_TAB_HEIGHT (LINES / 2)
#define TEXT_TAB_WIDTH (2 * COLS / 3)

#define WORDS_TAB_HEIGHT ((LINES / 2) - 5)
#define WORDS_TAB_WIDTH (2 * COLS / 3)

#define FREQUENCIES_TAB_HEIGHT (LINES - 6)
#define FREQUENCIES_TAB_WIDTH ((COLS / 3 - 2) / 2)

#define CTRL(x) ((x) & 0x1f)

void draw_frequencies_tab();

void draw_words_tab_frame();

void undo();

void redo();

void file_selector();

void draw_text_tab();

void draw_controls_tab();

void draw_words_by_letters_count();

void draw_words_by_decoded_letters_count();

void draw_word_selector();

void select_word();

void custom_match_input_window();

void draw_analyse_word_tab();

void analyse_word();

void word_analysis();

void draw_words_tab();

void main_page();

WINDOW *frequencies_tab;
WINDOW *expected_frequencies_tab;
WINDOW *words_tab;
WINDOW *text_tab;
WINDOW *controls_tab;
WINDOW *file_window;
WINDOW *custom_match_window;

void ui_init() {
    initscr();
    echo();
    start_color();

    init_pair(1, COLOR_BLACK, COLOR_YELLOW);
    init_pair(2, COLOR_WHITE, COLOR_RED);
    init_pair(3, COLOR_WHITE, COLOR_GREEN);

    state.height = LINES;
    state.width = COLS;
    state.show_decoded = 0;
    state.word_view_mode = VIEW_BY_LETTERS_COUNT;
    state.word_to_analyse_index = 0;
    state.word_to_analyse = calloc(sizeof(wchar_t), MAX_WORD_LENGTH);
    state.word_analysis_mode = SELECT_WORD;
    state.skip_input = 0;
    state.matching_word_index = 0;
    state.custom_match_words_count = 0;


    key_validity_message = calloc(sizeof(wchar_t), 30);

    text_tab = newwin(TEXT_TAB_HEIGHT, TEXT_TAB_WIDTH, 1, 1);
    controls_tab = newwin(4, COLS - 3, LINES - 5, 1);
    words_tab = newwin(WORDS_TAB_HEIGHT, WORDS_TAB_WIDTH, 1 + LINES / 2, 1);
    frequencies_tab = newwin(FREQUENCIES_TAB_HEIGHT, FREQUENCIES_TAB_WIDTH, 1, 2 * COLS / 3 + 1);
    expected_frequencies_tab = newwin(FREQUENCIES_TAB_HEIGHT, FREQUENCIES_TAB_WIDTH, 1,
                                      2 * COLS / 3 + (COLS / 3 - 2) / 2 + 1);
    custom_match_window = newwin(WORDS_TAB_HEIGHT, WORDS_TAB_WIDTH, 1 + LINES / 2, 1);
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

void draw_text_tab() {
    wclear(text_tab);
    wchar_t *string = (state.show_decoded) ? apply_key() : get_source_string();
    wchar_t *word = (state.show_decoded) ? get_decoded_words()[state.word_to_analyse_index]
                                         : get_words()[state.word_to_analyse_index];

    int max_length = TEXT_TAB_WIDTH - 4;
    const wchar_t *str_pos = string;
    int win_y = 2;
    int win_x = 2;
    size_t word_length = wcslen(word);

    int is_first_letter = 1;

    while (*str_pos) {
        if (*str_pos == L'\n') {
            win_y++;
            win_x = 2;
        } else {
            if ((str_pos + 1) && (win_x + first_word_len((str_pos + 1)) >= max_length)) {
                str_pos++;
                win_y++;
                win_x = 2;
            }

            // Check if the current position matches the word
            if (state.word_view_mode == WORD_ANALYSIS && word_length > 0 &&
                (is_first_letter || iswspace(*(str_pos - 1))) &&
                wcsncmp(str_pos, word, word_length) == 0
                && (iswspace(*(str_pos + word_length)) || *(str_pos + word_length) == L'\0' ||
                    iswpunct(*(str_pos + word_length)))) {
                // If the current position matches the word, highlight it
//                wattron(text_tab, A_STANDOUT);
                wattron(text_tab, COLOR_PAIR(3));
                mvwaddnwstr(text_tab, win_y, win_x, str_pos, word_length);
                wattroff(text_tab, COLOR_PAIR(3));
//                wattroff(text_tab, A_STANDOUT);
                str_pos += word_length - 1;
                win_x += word_length;
            } else {
                // Otherwise, write the character to the window without highlighting
                mvwaddnwstr(text_tab, win_y, win_x, str_pos, 1);
                win_x++;
            }

            is_first_letter = 0;
        }
        if (win_y >= TEXT_TAB_HEIGHT) {
            break;
        }
        str_pos++;
    }

    box(text_tab, 0, 0);
    mvwprintw(text_tab, 0, 1, (state.show_decoded) ? "Расшифрованный текст" : "Исходный текст");
    wrefresh(text_tab);
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
    wclear(words_tab);
    draw_words_tab_frame();
    int x = 2;
    int y = 2;

    wchar_t **words = (state.show_decoded) ? get_decoded_words() : get_words();

    for (int i = 0; i < get_words_count(); i++) {
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
    wclear(words_tab);
    draw_words_tab_frame();
    wchar_t **words = get_words_sorted_by_decoded_letters();


    int x = 2;
    int y = 2;
    for (int i = 0; i < get_words_count(); i++) {
        mvwprintw(words_tab, y, x, "%S ", words[i]);
        x += wcslen(words[i]) + 1;
        if (x + ((words[i + 1]) ? wcslen(words[i + 1]) : 0) >= (2 * COLS / 3) - 5) {
            y++;
            x = 2;
        }
    }
    wrefresh(words_tab);
}

void draw_word_selector() {
    wclear(words_tab);
    draw_words_tab_frame();
    do {
        draw_words_tab_frame();
        wchar_t **words = get_decoded_words();
        mvwprintw(words_tab, 0, WORDS_TAB_WIDTH / 2 - 7, " [Выбор слова] %S",
                  words[state.word_to_analyse_index]);

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
        draw_text_tab();

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
                break;
            case 'u':
                undo();
                draw_frequencies_tab();
                break;
            case CTRL('r'):
                redo();
                draw_frequencies_tab();
                break;
        }
        wcscpy(state.word_to_analyse, get_words()[state.word_to_analyse_index]);
    } while (state.word_view_mode == WORD_ANALYSIS && state.word_analysis_mode == SELECT_WORD);
    state.skip_input = 1;
}

void select_word() {
    draw_controls_tab();
    state.show_decoded = 1;
    draw_frequencies_tab();
    draw_word_selector();
}

void custom_match_input_window() {
    echo();
    curs_set(1);
    wchar_t message[] = L"Введите слово: ";
    wchar_t error_message[] = L"Длины слов не совпадают!";
    wchar_t mask_error_message[] = L"Введенное слово не совпадает с маской!   [ENTER] Подтвердить   [BACKSPACE] Ввести заново";
    wchar_t mask_strict_error_message[] = L"Буквы в слове не совпадают с ключом!   [ENTER] Подтвердить   [BACKSPACE] Ввести заново";
    wchar_t decoded_word[MAX_WORD_LENGTH] = L"";
    apply_key_to_str(state.word_to_analyse, decoded_word);

    wchar_t mask[MAX_WORD_LENGTH] = L"";
    wchar_t base_mask[MAX_WORD_LENGTH] = L"";
    generate_mask(decoded_word, mask);
    generate_mask(state.word_to_analyse, base_mask);

    wchar_t custom_match[MAX_WORD_LENGTH] = L"";

    do {
        wclear(custom_match_window);
        box(custom_match_window, 0, 0);
        mvwprintw(custom_match_window, 0, 1, "Добавление своего слова");
        mvwprintw(custom_match_window, 2, 1, "Исходное слово:       %S", state.word_to_analyse);
        mvwprintw(custom_match_window, 3, 1, "Расшифрованное слово: %S", decoded_word);
        mvwprintw(custom_match_window, 4, 1, "Маска слова:          %S (%S)", mask, base_mask);
        mvwprintw(custom_match_window, 5, 1, "Новое слово:          ");
        wrefresh(custom_match_window);

        if (!wcslen(custom_match)) continue;

        if (wcslen(custom_match) == wcslen(state.word_to_analyse)) {
            match_type match = does_match_mask(custom_match, mask);
            if (match != STRICT_MATCH) {
                wattron(custom_match_window, A_STANDOUT);
                mvwaddwstr(custom_match_window, WORDS_TAB_HEIGHT - 2, 2,
                           ((match) ? mask_strict_error_message : mask_error_message));
                wattroff(custom_match_window, A_STANDOUT);
                wrefresh(custom_match_window);
                switch (getch()) {
                    case '\n':
                        break;
                    default:
                        continue;
                        break;
                }
            }
            add_key_to_history();
            generate_key_from_matches(state.word_to_analyse, custom_match);
            break;
        }
        wattron(custom_match_window, A_STANDOUT);
        mvwaddwstr(custom_match_window, WORDS_TAB_HEIGHT - 2, 2, error_message);
        wattroff(custom_match_window, A_STANDOUT);
    } while (mvwscanw(custom_match_window, 5, 23, "%S", custom_match) != -1);
    curs_set(0);
    noecho();
}

void undo() {
    undo_key_change();
    apply_key();
}

void redo() {
    redo_key_change();
    apply_key();
}

void draw_analyse_word_tab() {
    wclear(words_tab);
    state.show_decoded = 1;
    wchar_t matching_word[MAX_WORD_LENGTH] = L"";
    state.matching_word_index = 0;
    do {
        draw_words_tab_frame();
        mvwprintw(words_tab, 0, WORDS_TAB_WIDTH / 2 - (wcslen(state.word_to_analyse) + 2) / 2, "[%S]",
                  state.word_to_analyse);
        wchar_t decoded_word[MAX_WORD_LENGTH] = L"";
        apply_key_to_str(state.word_to_analyse, decoded_word);
        wchar_t mask[ALPHABET_SIZE] = L"";
        generate_mask(decoded_word, mask);
        mvwprintw(words_tab, 2, 2, "%S (%S), %d", decoded_word, mask, state.matching_word_index);
        mvwhline(words_tab, WORDS_TAB_HEIGHT / 4 - 1, 1, 0, WORDS_TAB_WIDTH - 2);
        int x = 2;
        int y = WORDS_TAB_HEIGHT / 4 + 1;
        int word_count = 0;

        int has_not_strict_matches = 0;

        for (int i = 0; i < WORDLIST_LENGTH; i++) {
            match_type match = does_match_mask(FREQUENT_WORDS_RU[i], mask);
            if (!match) continue;
            char *format = (word_count == state.matching_word_index) ? "[%S] " : " %S  ";
            if (word_count++ == state.matching_word_index) {
                wcscpy(matching_word, FREQUENT_WORDS_RU[i]);
            }
            if (match != STRICT_MATCH) {
                wattron(words_tab, COLOR_PAIR(1));
                has_not_strict_matches = 1;
            }
            mvwprintw(words_tab, y, x, format, FREQUENT_WORDS_RU[i]);
            wattroff(words_tab, COLOR_PAIR(1));
            x += wcslen(FREQUENT_WORDS_RU[i]) + 2;
            if (x + ((FREQUENT_WORDS_RU[i + 1]) ? wcslen(FREQUENT_WORDS_RU[i + 1]) : 0) >= (2 * COLS / 3) - 5) {
                y++;
                x = 2;
            }
        }

        if (has_not_strict_matches) {
            wattron(words_tab, COLOR_PAIR(1));
            mvwprintw(words_tab, WORDS_TAB_HEIGHT - 2, 2, "Желтые слова создадут дубликаты букв при выборе!");
            wattroff(words_tab, COLOR_PAIR(1));
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
            case '\n':
                add_key_to_history();
                generate_key_from_matches(state.word_to_analyse, matching_word);
                apply_key();
                draw_text_tab();
                draw_frequencies_tab();
                state.word_analysis_mode = SELECT_WORD;
                break;
            case ' ':
                custom_match_input_window();
                apply_key();
                wrefresh(text_tab);
                wrefresh(frequencies_tab);
                state.word_analysis_mode = SELECT_WORD;
                break;
            case 'u':
                undo();
                break;
            case CTRL('r'):
                redo();
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
    wclear(frequencies_tab);
    double *frequencies = get_frequencies();

    int *key = get_key_ptr();

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        mvwprintw(frequencies_tab, 1 + i, 2, "%C = %lf", ALPHABET_RU[state.indexes[i]], frequencies[state.indexes[i]]);
        wchar_t suggested_letter = (key[state.indexes[i]] > -1) ? ALPHABET_RU[key[state.indexes[i]]] : L'?';
        if (key[state.indexes[i]] != -1 && !IS_UNIQUE(key, ALPHABET_SIZE, key[state.indexes[i]])) {
            wattron(frequencies_tab, COLOR_PAIR(2));
        }

        if (state.word_view_mode != WORD_ANALYSIS && i == state.active_letter)
            mvwprintw(frequencies_tab, 1 + i, 14, " [%C]", suggested_letter);
        else
            mvwprintw(frequencies_tab, 1 + i, 14, "  %C ", suggested_letter);

        wattroff(frequencies_tab, COLOR_PAIR(2));
    }


    for (int i = 0; i < ALPHABET_SIZE; i++) {
        mvwprintw(expected_frequencies_tab, 1 + i, 2, "%C = %lf\n", ALPHABET_RU[state.expected_indexes[i]],
                  FREQUENCIES_RU[state.expected_indexes[i]]);
    }

    if (!DOES_CONTAIN(key, ALPHABET_SIZE, -1)) {
        apply_key();
        wcscpy(key_validity_message, ((is_key_valid()) ? L"Ключ вероятно верный" : L"Ключ вероятно неверный"));
        wattron(frequencies_tab, A_STANDOUT);
        mvwaddwstr(frequencies_tab, FREQUENCIES_TAB_HEIGHT - 2,
                   ((FREQUENCIES_TAB_WIDTH - wcslen(key_validity_message)) / 2), key_validity_message);
        wattroff(frequencies_tab, A_STANDOUT);
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
                case 'u':
                    undo();
                    break;
                case CTRL('r'):
                    redo();
                    break;
                case 's':
                    add_key_to_history();
                    break;
            }
        }
        state.skip_input = 0;
        draw_text_tab();
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
