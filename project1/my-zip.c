/* Joonatan Rantanen 0518320 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    int i, j, nbr, nextidx = 0, arrsize = 100;
    int *ints = malloc(arrsize * sizeof(int));
    char *ptr, *lineptr = NULL, *chars = malloc(arrsize);
    size_t n = 0;
    FILE *f;
    
    if (argc < 2) {
        printf("my-zip: file1 [file2 ...]\n");
        free(ints);
        free(chars);
        exit(1);
    }
    
    for (i = 1; i < argc; i++) {
        f = fopen(argv[i], "r");
        
        if (f == NULL) {
            printf("my-zip: cannot open file\n");
            free(lineptr);
            free(ints);
            free(chars);
            exit(1);
        }
        
        while (getline(&lineptr, &n, f) != -1) {
            if (arrsize + strlen(lineptr) + 1 > arrsize) {
                arrsize += strlen(lineptr) + 1;
                chars = realloc(chars, arrsize);
                ints = realloc(ints, arrsize * sizeof(int));
            }
            
            ptr = lineptr;
            
            while (*ptr != '\0') {
                nbr = 1;
                
                while (*ptr == *(ptr + 1)) {
                    nbr++;
                    ptr++;
                }
                
                /* nums and chars are saved in arrays before writing them 
                 * anywhere, because otherwise if one file ends in the same 
                 * char as next begins with, the result file isn't optimal
                 * and could have something like "3a5a"
                 */
                ints[nextidx] = nbr;
                chars[nextidx] = *ptr;
                nextidx++;
                ptr++;
            }
        }
        
        fclose(f);
    }
    
    /* if there's a case similar to "3a5a" or even "3a5a2a" etc, sum all those
     * integers in the first one and "delete" the others by setting them to -1
     * so they won't be used
     */
    for (i = 0; i < nextidx - 1; i++) {
        if (chars[i] == chars[i + 1]) {
            /* find the earliest not-deleted occurrence of this char */
            for (j = i; ints[j] < 0;) {
                j--;
            }
            
            ints[j] += ints[i + 1];
            ints[i + 1] = -1;
        }
    }
    
    for (i = 0; i < nextidx; i++) {
        if (ints[i] != -1) { /* "deleted" ints are -1 */
            fwrite(ints + i, 4, 1, stdout);
            fwrite(chars + i, 1, 1, stdout);
        }
    }
    
    free(ints);
    free(chars);
    free(lineptr);
    return 0;
}

