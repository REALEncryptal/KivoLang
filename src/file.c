#include <stdio.h>
#include <stdlib.h>

// Reads file into a buffer and returns it
char *read_file(FILE *file) {
    rewind(file); // reset everything

    fseek(file, 0, SEEK_END); // go to end of file
    long size = ftell(file); // returns the byte offset but since we are at the end it also means the size

    rewind(file); // go back to start

    // allocate enough memory to store the entirer file as well as \0 
    char *source = malloc(size + 1);
    size_t bytes_read = fread(source, 1, size, file); // read into our buffer
    
    // add \0 
    source[bytes_read] = '\0';// bytes read is important cause things like errors might change the size

    return source;
}

char *get_source(const char *filename) {
    FILE *file = NULL;

    // fopen_s only exists on MSVC; everywhere else use plain fopen
#ifdef _MSC_VER
    if (fopen_s(&file, filename, "r") != 0) {
        return NULL;
    }
#else
    file = fopen(filename, "r");
    if (file == NULL) {
        return NULL;
    }
#endif

    char *source = read_file(file);
    fclose(file);

    return source;
}
