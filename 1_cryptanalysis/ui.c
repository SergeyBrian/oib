#include "ui.h"

static ui_state state;
static const wchar_t letters_controls_text[] = L"[Tab] Режим просмотра текста  [↑]/[↓] Выбор буквы  "
                                               "[←]/[→] Изменить выбранную букву  [BACKSPACE] Сбросить выбранную букву";
static const wchar_t select_word_controls_text[] = L"[←]/[→] Выбор слова   [ENTER] Подтвердить выбор";
static const wchar_t analyse_word_controls_text[] = L"[BACKSPACE] Выбрать другое слово   [SPACE] Ввести свое слово   [←]/[→] Выбор слова   [ENTER] Подтвердить выбор";
static const wchar_t common_controls_text[] = L"[W] Режим просмотра слов   [Q] Выйти";
static const wchar_t auto_generation_message[] = L"Чтобы начать автоподбор ключа, нажмите [ENTER] (Эту операцию нельзя будет прервать)";
static const wchar_t auto_generation_label[] = L"Автоматический подбор ключа";
static const wchar_t auto_generation_pass_message[] = L"Проход #";

static wchar_t *key_validity_message;

#define TEXT_TAB_HEIGHT (LINES / 2)
#define TEXT_TAB_WIDTH (2 * COLS / 3)

#define WORDS_TAB_HEIGHT ((LINES / 2) - 5)
#define WORDS_TAB_WIDTH (2 * COLS / 3)

#define FREQUENCIES_TAB_HEIGHT (LINES - 6)
#define FREQUENCIES_TAB_WIDTH ((COLS / 3 - 2) / 2)

#define PROGRESS_WINDOW_HEIGHT 4
#define PROGRESS_WINDOW_WIDTH (COLS/2)

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
WINDOW *auto_generation_window;
WINDOW *progress_bar_window;

void ui_init() {
    initscr();
    echo();
    start_color();
    curs_set(0);

    init_pair(1, COLOR_BLACK, COLOR_YELLOW);
    init_pair(2, COLOR_WHITE, COLOR_RED);
    init_pair(3, COLOR_WHITE, COLOR_GREEN);
    init_pair(4, COLOR_BLACK, COLOR_WHITE);

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
    auto_generation_window = newwin(LINES, COLS, 0, 0);
    progress_bar_window = newwin(PROGRESS_WINDOW_HEIGHT, PROGRESS_WINDOW_WIDTH, LINES - 5,
                                 (COLS - PROGRESS_WINDOW_WIDTH) / 2);
}

void ui_set_page(ui_page page) {
    state.current_page = page;
}


void file_selector(int wordlist_required) {
    curs_set(1);
    echo();
    file_window = newwin(LINES / 2, COLS / 2, LINES / 4, COLS / 4);
    char input_filename[1000];
    char wordlist_filename[1000];
    int is_input_active = 0;
    int show_file_open_error = 0;

    while (state.current_page == FILE_SELECTOR) {
        while (!state.is_input_file_open) {
            wclear(file_window);
            box(file_window, 0, 0);

            if (show_file_open_error) {
            }

            mvwprintw(file_window, 2, 2, "Input file name: ");
            wrefresh(file_window);
            wscanw(file_window, "%s", input_filename);

            if (open_file(input_filename)) {
                state.current_page = MAIN_PAGE;
                show_file_open_error = 0;
                state.is_input_file_open = 1;
                break;
            }

            show_file_open_error = 1;
        }

        do {
            wclear(file_window);
            box(file_window, 0, 0);

            if (show_file_open_error) {
                mvwprintw(file_window, 0, 2, "Can't open file!");
            }

            mvwprintw(file_window, 2, 2, "Input file name: %s", input_filename);
            mvwprintw(file_window, 3, 2, "Wordlist file name (leave blank to use default wordlist): ");
            wrefresh(file_window);


            if (wscanw(file_window, "%s", wordlist_filename) == -1) {
                state.current_page = MAIN_PAGE;
                break;
            }
            if (open_wordlist(wordlist_filename)) {
                read_wordlist();
                state.is_wordlist_file_open = 1;
                state.current_page = MAIN_PAGE;
                show_file_open_error = 0;
                break;
            }
            show_file_open_error = 1;
        } while (wordlist_required && !state.is_wordlist_file_open);
    }
    curs_set(0);
    noecho();
    delwin(file_window);
}

void draw_progress_bar(double progress, int pass) {
    wclear(progress_bar_window);
    box(progress_bar_window, 0, 0);
    int real_progress = (int) (progress * (double) (PROGRESS_WINDOW_WIDTH));
    mvwprintw(progress_bar_window, 0, 2, "%S%d", auto_generation_pass_message, pass);
    wattron(progress_bar_window, COLOR_PAIR(4));
    for (int i = 1; i < real_progress; i++) {
        mvwaddch(progress_bar_window, 2, i, ' ');
    }
    wattroff(progress_bar_window, COLOR_PAIR(4));
    wrefresh(progress_bar_window);
}

void draw_auto_generation_tab() {
    if (!state.is_wordlist_file_open) {
        state.current_page = FILE_SELECTOR;
        file_selector(1);
    }
    if (!state.is_wordlist_file_open) return;

    clear();
    refresh();
    mvwprintw(auto_generation_window, LINES / 2, (COLS - wcslen(auto_generation_message)) / 2, "%S",
              auto_generation_message);
    wrefresh(auto_generation_window);
    if (getch() != '\n') return;
    wclear(auto_generation_window);
    box(auto_generation_window, 0, 0);
    mvwprintw(auto_generation_window, 0, 2, "%S", auto_generation_label);
    wrefresh(auto_generation_window);
    auto_generate_key(draw_progress_bar);
    state.show_decoded = 1;
    clear();
    refresh();
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
                wattron(text_tab, COLOR_PAIR(3));
                mvwaddnwstr(text_tab, win_y, win_x, str_pos, word_length);
                wattroff(text_tab, COLOR_PAIR(3));
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
                wattron(custom_match_window, COLOR_PAIR(2));
                mvwaddwstr(custom_match_window, WORDS_TAB_HEIGHT - 2, 2,
                           ((match) ? mask_strict_error_message : mask_error_message));
                wattroff(custom_match_window, COLOR_PAIR(2));
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
            state.word_analysis_mode = SELECT_WORD;
            curs_set(0);
            noecho();
            return;
            break;
        }
        wattron(custom_match_window, COLOR_PAIR(2));
        mvwaddwstr(custom_match_window, WORDS_TAB_HEIGHT - 2, 2, error_message);
        wattroff(custom_match_window, COLOR_PAIR(2));
    } while (mvwscanw(custom_match_window, 5, 23, "%S", custom_match) != -1);
    state.word_analysis_mode = ANALYSE_WORD;
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
        mvwprintw(words_tab, 2, 2, "%S (%S)", decoded_word, mask);
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
            mvwprintw(frequencies_tab, 1 + i, 14, " [%C] ", suggested_letter);
        else
            mvwprintw(frequencies_tab, 1 + i, 14, "  %C ", suggested_letter);

        wattroff(frequencies_tab, COLOR_PAIR(2));
    }


    for (int i = 0; i < ALPHABET_SIZE; i++) {
        mvwprintw(expected_frequencies_tab, 1 + i, 2, "%C = %lf\n", ALPHABET_RU[state.expected_indexes[i]],
                  FREQUENCIES_RU[state.expected_indexes[i]]);
    }

    if (INDEX_OF(key, ALPHABET_SIZE, -1) == -1) {
        apply_key();
        int key_valid = is_key_valid();
        wcscpy(key_validity_message, ((key_valid) ? L"Ключ вероятно верный" : L"Ключ вероятно неверный"));
        wattron(frequencies_tab, COLOR_PAIR((key_valid) ? 3 : 1));
        mvwaddwstr(frequencies_tab, FREQUENCIES_TAB_HEIGHT - 2,
                   ((FREQUENCIES_TAB_WIDTH - wcslen(key_validity_message)) / 2), key_validity_message);
        wattroff(frequencies_tab, COLOR_PAIR(1));
        wattroff(frequencies_tab, COLOR_PAIR(3));
    }

    mvwhline(frequencies_tab, FREQUENCIES_TAB_HEIGHT - 15, 1, 0, FREQUENCIES_TAB_WIDTH - 2);

    for (int i = 0; i < BIGRAMS_COUNT; i++) {
        mvwprintw(frequencies_tab, FREQUENCIES_TAB_HEIGHT - 14 + i, 2, "%S = %lf",
                  ((state.show_decoded) ? get_decoded_bigrams()[state.bigrams_indexes[i]]
                                        : get_bigrams()[state.bigrams_indexes[i]]),
                  get_bigrams_frequencies()[state.bigrams_indexes[i]]);
    }

    mvwhline(expected_frequencies_tab, FREQUENCIES_TAB_HEIGHT - 15, 1, 0, FREQUENCIES_TAB_WIDTH - 2);

    for (int i = 0; i < BIGRAMS_COUNT; i++) {
        mvwprintw(expected_frequencies_tab, FREQUENCIES_TAB_HEIGHT - 14 + i, 2, "%S = %lf",
                  FREQUENT_BIGRAMS_RU[state.bigrams_expected_indexes[i]],
                  BIGRAMS_FREQUENCIES_RU[state.bigrams_expected_indexes[i]]);
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
    state.is_input_file_open = 1;
    keypad(stdscr, true);
    analysis_init();
    sort_indexes(get_frequencies(), state.indexes);
    sort_indexes(FREQUENCIES_RU, state.expected_indexes);

    sort_indexes(get_bigrams_frequencies(), state.bigrams_indexes);
    sort_indexes(BIGRAMS_FREQUENCIES_RU, state.bigrams_expected_indexes);

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
                case 'a':
                    draw_auto_generation_tab();
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
            file_selector(0);
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
    curs_set(1);
    endwin();
}

void ui_set_wordlist_open() {
    state.is_wordlist_file_open = 1;
}
