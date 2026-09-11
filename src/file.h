#ifndef KIVO_FILE_H
#define KIVO_FILE_H

#include <stdio.h>

char *readFile(FILE *file);
char *getSource(const char *filename);

#endif