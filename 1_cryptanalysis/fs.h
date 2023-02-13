#ifndef INC_1_CRYPTANALYSIS_FS_H
#define INC_1_CRYPTANALYSIS_FS_H

#include <stdlib.h>
#include <stdio.h>
#include <wctype.h>

#include "utils.h"


int open_file(char *filename);
wchar_t *readfile();

#endif //INC_1_CRYPTANALYSIS_FS_H
