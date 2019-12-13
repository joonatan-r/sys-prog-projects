/* Joonatan Rantanen 0518320 */

#include <stdio.h>
#include <stdlib.h>

#define MAXLENGTH 1000

int main(int argc, char **argv) {
    int i;
    
    if (argc < 2) {
        return 0;
    }
    
    for (i = 1; i < argc; i++) {
        FILE *f = fopen(argv[i], "r");
        char buffer[MAXLENGTH];
        
        if (f == NULL) {
            printf("my-cat: cannot open file\n");
            exit(1);
        }
        
        while (fgets(buffer, MAXLENGTH, f) != NULL) {
            printf("%s", buffer);
        }
        
        fclose(f);
    }
    
    return 0;
}

