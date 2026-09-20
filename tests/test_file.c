#include "file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int failures = 0;

#define CHECK(condition)                                                        \
    do {                                                                        \
        if (!(condition)) {                                                     \
            fprintf(stderr, "%s:%d: check failed: %s\n",                      \
                    __FILE__, __LINE__, #condition);                            \
            failures++;                                                         \
        }                                                                       \
    } while (0)

static void test_read_file_reads_from_the_beginning(void) {
    FILE *file = tmpfile();
    const char contents[] = "alpha\nbeta";

    CHECK(file != NULL);
    if (file == NULL) return;

    CHECK(fwrite(contents, 1, sizeof(contents) - 1, file) ==
          sizeof(contents) - 1);
    CHECK(fseek(file, 5, SEEK_SET) == 0);

    char *source = read_file(file);
    CHECK(source != NULL);
    if (source != NULL) {
        CHECK(strcmp(source, contents) == 0);
        free(source);
    }

    fclose(file);
}

static void test_read_file_handles_an_empty_file(void) {
    FILE *file = tmpfile();

    CHECK(file != NULL);
    if (file == NULL) return;

    char *source = read_file(file);
    CHECK(source != NULL);
    if (source != NULL) {
        CHECK(source[0] == '\0');
        free(source);
    }

    fclose(file);
}

static void test_read_file_preserves_embedded_nuls(void) {
    FILE *file = tmpfile();
    const unsigned char contents[] = {'a', 'b', '\0', 'c', 'd'};

    CHECK(file != NULL);
    if (file == NULL) return;

    CHECK(fwrite(contents, 1, sizeof(contents), file) == sizeof(contents));

    char *source = read_file(file);
    CHECK(source != NULL);
    if (source != NULL) {
        CHECK(memcmp(source, contents, sizeof(contents)) == 0);
        CHECK(source[sizeof(contents)] == '\0');
        free(source);
    }

    fclose(file);
}

static void test_get_source_reads_an_existing_file(void) {
    char *source = get_source("tests/fixtures/file_sample.txt");

    CHECK(source != NULL);
    if (source != NULL) {
        CHECK(strcmp(source, "first line\nsecond line\n") == 0);
        free(source);
    }
}

static void test_get_source_reports_a_missing_file(void) {
    char *source = get_source("tests/fixtures/does-not-exist.kvo");

    CHECK(source == NULL);
    free(source);
}

int main(void) {
    test_read_file_reads_from_the_beginning();
    test_read_file_handles_an_empty_file();
    test_read_file_preserves_embedded_nuls();
    test_get_source_reads_an_existing_file();
    test_get_source_reports_a_missing_file();

    if (failures != 0) {
        fprintf(stderr, "%d file test(s) failed\n", failures);
        return EXIT_FAILURE;
    }

    puts("file tests passed");
    return EXIT_SUCCESS;
}
