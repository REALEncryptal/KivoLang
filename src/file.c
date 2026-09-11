#include <stdio.h>
#include <stdlib.h>

// Reads file into a buffer and returns it
char *readFile(FILE *file) {
    rewind(file); // reset everything

    fseek(file, 0, SEEK_END); // go to end of file
    long size = ftell(file); // returns the byte offset but since we are at the end it also means the size

    rewind(file); // go back to start

    // allocate enough memory to store the entirer file as well as \0 
    char *source = malloc(size + 1);
    fread(source, 1, size, file); // read into our buffer

    return source;
}

char *getSource(const char *filename) {
    FILE *file = NULL;

    if (fopen_s(&file, filename, "r") != 0) {
        return NULL;
    }

    char *source = readFile(file);
    fclose(file);

    return source;
}