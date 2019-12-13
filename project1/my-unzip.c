/* Joonatan Rantanen 0518320 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int i;
    
    if (argc < 2) {
        printf("my-unzip: file1 [file2 ...]\n");
        exit(1);
    }
    
    for (i = 1; i < argc; i++) {
        FILE *f = fopen(argv[i], "r");
        int nbr;
        char chr;
        
        if (f == NULL) {
            printf("my-unzip: cannot open file\n");
            exit(1);
        }
        
        while (fread(&nbr, 4, 1, f) != 0 && fread(&chr, 1, 1, f) != 0) {
            for (; nbr > 0; nbr--) {
                printf("%c", chr);
            }
        }
        
        fclose(f);
    }    
    
    return 0;
}

