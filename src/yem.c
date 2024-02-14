#include <stdio.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("incorrect usage\n");
    }
    for (int i = 0; i < argc; ++i) {
        printf("%s\n", argv[i]);
    }
    return 0;
}
