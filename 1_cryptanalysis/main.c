#include <locale.h>

#include "ui.h"
#include "utils.h"
#include "fs.h"

typedef enum {
    INPUT_FILE_NAME,
    OUTPUT_FILE_NAME,
    GENERATE
} param;

void parse_arguments(int argc, char **argv, char **params_ptr) {
    param current_param;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'i':
                    current_param = INPUT_FILE_NAME;
                    break;
                case 'o':
                    current_param = OUTPUT_FILE_NAME;
                    break;
                case 'g':
                    current_param = GENERATE;
                    break;
                default:
                    error_exit("Incorrect command line arguments");
            }
            continue;
        }

        params_ptr[current_param] = &argv[i][0];
    }
}

void on_user_input(int c) {
    switch(c) {
        case 'q':
            quit();
        default:
            break;
    }
}

int main(int argc, char **argv) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    ui_init();
    ui_set_page(FILE_SELECTOR);

    while (getch() != 'q') {
        ui_update();
    }

    quit();
    return 0;
}
