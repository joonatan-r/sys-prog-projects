/* Joonatan Rantanen 0518320 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    int i;
    char *lineptr = NULL;
    size_t n = 0;
    
    if (argc < 2) {
        printf("my-grep: searchterm [file ...]\n");
        exit(1);
    }
    
    if (argc == 2) {
        while (getline(&lineptr, &n, stdin) != -1) {
            if (strstr(lineptr, argv[1]) != NULL) {
                printf("%s", lineptr);
            }
        }
    }
    
    for (i = 2; i < argc; i++) {
        FILE *f = fopen(argv[i], "r");
        
        if (f == NULL) {
            printf("my-grep: cannot open file\n");
            free(lineptr);
            exit(1);
        }
        
        while (getline(&lineptr, &n, f) != -1) {
            if (strstr(lineptr, argv[1]) != NULL) {
                printf("%s", lineptr);
            }
        }
        
        fclose(f);
    }
    
    free(lineptr);
    return 0;
}

