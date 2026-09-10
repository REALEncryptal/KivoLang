#include <stdio.h>

int main (int argc, char *argv[]) {
    
    printf("[===== KIVO =====]\n");

    // Oopen the testing source
    FILE *test_file = fopen("input.kvo", "r");
    if (test_file == NULL) {
        printf("Error: Could not open input.kvo\n");
        return 1;
    }

    

    return 0;
}
