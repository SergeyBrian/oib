#include <locale.h>
#include <string.h>

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

int main(int argc, char **argv) {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    ui_init();
    ui_set_page(FILE_SELECTOR);
    char *params[20] = {0};

    if (argc > 1) {
        parse_arguments(argc, argv, params);
    }

    if (params[INPUT_FILE_NAME] != NULL) {
        if (open_file(params[INPUT_FILE_NAME])) {
            ui_set_page(MAIN_PAGE);
        }
    }


    do {
        ui_update();
    } while (1);

    quit();
    return 0;
}
