#ifndef INC_1_CRYPTANALYSIS_FS_H
#define INC_1_CRYPTANALYSIS_FS_H

#include <stdlib.h>
#include <stdio.h>
#include <wctype.h>

#include "utils.h"


int open_file(char *filename);
int open_wordlist(char *filename);

wchar_t *readfile();
wchar_t **read_wordlist();

#endif //INC_1_CRYPTANALYSIS_FS_H
