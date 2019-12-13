/* Joonatan Rantanen 0518320 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

int create_child(char **args, char *redirectfile) {
    switch(fork()) {
        case -1:
            return -1;
        case 0:
            if (redirectfile != NULL) {
                int fd = open(redirectfile, O_WRONLY | O_CREAT | O_TRUNC, 
                              S_IRUSR | S_IWUSR);
                
                if (fd < 0) {
                    return 1;
                }
                
                dup2(fd, STDOUT_FILENO);
                dup2(fd, STDERR_FILENO);
                close(fd);
            }
            
            if (execv(*args, args) == -1) {
                return 2;
            }
            break;
        default:
            return 0;
    }
    return 0;
}

void print_error(char *error_message) {
    write(STDERR_FILENO, error_message, strlen(error_message));
}

int main(int argc, char **argv) {
    int i, j, k, count, status, do_main_loop = 1, pathitems = 1, 
        idxarritems = 0, children = 0, fileitems, initial, initial_redirect;
    char *cmd = NULL, *token, *ptr, *redirectfile, *program_name, *saveptr, 
         *token1, *saveptr1, *token0, *saveptr0;
    unsigned int argssize = 10, pathsize = 5, namesize = 10, idxarrsize = 10, 
                 filearrsize = 10;
    int *first_args, *file_to_arg;
    char **args, **path, **files;
    size_t n = 0;
    FILE *input;
    
    if (argc > 2) {
        print_error("Error: Too many arguments\n");
        exit(1);
    }
    
    if (argc == 2) {
        if ((input = fopen(argv[1], "r")) == NULL) {
            print_error("Failed to open input file\n");
            exit(1);
        }
    } else {
        input = stdin;
    }
    
    /* holds path + executable name */
    program_name = malloc(namesize);
    /* array for all args from the cmd line */
    args = malloc(argssize * sizeof(char *));
    /* array that holds idxs for each first argument 
     * (for groups separated by '&'s) in args
     */
    first_args = malloc(idxarrsize * sizeof(int *));
    /* array for directories in path; the only array that holds strings, 
     * all others are just pointers to cmd line
     */
    path = malloc(pathsize * sizeof(char *));
    path[0] = malloc(5);
    strcpy(path[0], "/bin"); /* pathitems is initially set to one also */
    /* array that holds files where output is directed */
    files = malloc(filearrsize * sizeof(char *));
    /* array that keeps track what argument groups had what redirect files */
    file_to_arg = malloc(filearrsize * sizeof(int));
    
    while (do_main_loop) {
        if (argc < 2) printf("wish> ");
        
        if (getline(&cmd, &n, input) == -1) {
            if (feof(input) == 0) {
                print_error("Failed to get command\n");
            } else {
                break;
            }
        }
        
        if (strcmp(cmd, "\n") == 0) continue;
        
        cmd = strtok(cmd, "\n");
        i = 0; /* keeps track of number of items in args */
        idxarritems = 0; /* items in first_args */
        fileitems = 0; /* items in files */
        /* variables that keep track how many items are in array are also 
         * the next idx for item to add
         */
        
        /* each argument group separated by '&'s is handled separately, 
         * excess '&'s are ignored
         */
        for (token0 = strtok_r(cmd, "&", &saveptr0); token0 != NULL; 
             token0 = strtok_r(NULL, "&", &saveptr0)) {
            
            count = 0;
            initial = 1;
            initial_redirect = 0;
            
            for (ptr = token0; *ptr != '\0'; ptr++) {
                if (*ptr == '>') {
                    count++;
                    
                    if (initial) initial_redirect = 1;
                }
                
                if (*ptr != ' ' && *ptr != '\t') initial = 0;
            }
            
            if (count > 1) {
                print_error("Error: Multiple redirect symbols not allowed\n");
                continue;
            }
            
            if (initial_redirect) {
                print_error("Error: No command to redirect\n");
                continue;
            }
            
            token0 = strtok(token0, ">");
            redirectfile = strtok(NULL, ">");
            
            if (redirectfile != NULL) {
                j = 0;
                
                for (token1 = strtok_r(redirectfile, "\t", &saveptr1); 
                     token1 != NULL; token1 = strtok_r(NULL, "\t", &saveptr1)) {
                    for (token = strtok_r(token1, " ", &saveptr); 
                         token != NULL; token = strtok_r(NULL, " ", &saveptr)) {
                        /* the first word after '>' (words separated by some 
                         * number of spaces or tabs or both)
                         */
                        if (!j) redirectfile = token;
                        
                        j++; /* detect if there are multiple files given */
                    }
                }
                
                if (j > 1) {
                    print_error("Error: Only one redirect file allowed\n");
                    continue;
                } else {
                    /* if arrays are too small, double their size so we don't 
                     * have to realloc all the time
                     */
                    while (fileitems + 1 > filearrsize) {
                        filearrsize *= 2;
                        files = realloc(files, filearrsize * sizeof(char *));
                        file_to_arg = realloc(file_to_arg, 
                                              filearrsize * sizeof(int));
                    }
                    
                    files[fileitems] = redirectfile;
                    /* file_to_arg is updated when we save first arg idx */
                }
            } else if (count > 0) { /* '>' without file given */
                print_error("Error: No redirect file given\n");
                continue;
            }
            
            /* arguments separated by some number of spaces or tabs or both */
            for (token1 = strtok_r(token0, "\t", &saveptr1); token1 != NULL; 
                 token1 = strtok_r(NULL, "\t", &saveptr1)) {
                for (token = strtok_r(token1, " ", &saveptr); token != NULL; 
                     token = strtok_r(NULL, " ", &saveptr)) {
                    while (i + 2 > argssize) {
                        argssize *= 2;
                        args = realloc(args, argssize * sizeof(char *));
                    }
                    
                    if (i == 0 || args[i - 1] == NULL) {
                        /* first arg in this group */
                        while (idxarritems + 1 > idxarrsize) {
                            idxarrsize *= 2;
                            first_args = realloc(first_args, 
                                                 idxarrsize * sizeof(int *));
                        }
                        
                        first_args[idxarritems] = i;
                        idxarritems++;
                        
                        if (redirectfile != NULL) {
                            file_to_arg[fileitems] = first_args[idxarritems-1];
                            fileitems++;
                        }
                    }
                    
                    args[i] = token;
                    i++;
                }
            }
            
            /* NULL pointer after group so exec knows where its args end */
            args[i] = NULL;
            i++;
        }
        
        if (i == 0 || args[0] == NULL) continue;
        
        for (k = 0; k < idxarritems; k++) { /* handle all arg groups */
            int idx = first_args[k];
            redirectfile = NULL;
            
            for (j = 0; j < fileitems; j++) {
                if (file_to_arg[j] == idx) {
                    redirectfile = files[j];
                    break;
                }
            }
            
            if (strcmp(args[idx], "exit") == 0) {
                if (args[idx + 1] != NULL) {
                    print_error("Error: \"exit\" takes no arguments\n");
                } else {
                    do_main_loop = 0;
                    break;
                }
            } else if (strcmp(args[idx], "cd") == 0) {
                if (args[idx + 1] == NULL || 
                    (i > idx + 2 && args[idx + 2] != NULL)) {
                    
                    print_error("Error: \"cd\" takes exactly one argument\n");
                } else {
                    if (chdir(args[1]) != 0) {
                        print_error("Failed to change directory\n");
                    }
                }
            } else if (strcmp(args[idx], "path") == 0) {
                for (j = idx + 1; args[j] != NULL; j++) {
                    while (j + 1 > pathsize) {
                        pathsize *= 2;
                        path = realloc(path, pathsize * sizeof(char *));
                    }
                    
                    if (j - 1 < pathitems) {
                        /* if there was something in this idx, delete it */
                        free(path[j - 1]);
                        path[j - 1] = NULL;
                    }
                    
                    path[j - 1] = malloc(strlen(args[j]) + 1);
                    strcpy(path[j - 1], args[j]);
                }
                
                for (k = j - 1; k < pathitems; k++) {
                    /* if old path array was longer, delete old items */
                    free(path[k]);
                    path[k] = NULL;
                }
                
                pathitems = j - 1;
            } else {
                for (j = 0; j < pathitems; j++) {
                    while (strlen(path[j]) + strlen(args[idx]) + 2 > namesize) {
                        namesize *= 2;
                        program_name = realloc(program_name, namesize);
                    }
                    
                    sprintf(program_name, "%s/%s", path[j], args[idx]);
                    
                    if (access(program_name, X_OK) == 0) {
                        args[idx] = program_name;
                        
                        switch (create_child(args + idx, redirectfile)) {
                            case -1:
                                print_error("Failed to create process "
                                            "for command\n");
                                break;
                            case 0:
                                children++;
                                break;
                            case 1:
                                print_error("Failed to open redirect file\n");
                                do_main_loop = 0;
                                /* if child process has error, it exits */
                                break;
                            case 2:
                                print_error("Failed to execute command\n");
                                do_main_loop = 0;
                        }
                        
                        break;
                    }
                }
                
                if (j == pathitems) {
                    print_error("No such command in path\n");
                }
            }
        }
        
        while (children > 0) {
            if (wait(&status) == -1) {
                print_error("Waiting for child process resulted in error\n");
            }
            
            children--;
        }
    }
    
    if (argc == 2) {
        fclose(input);
    }
    
    free(files);
    free(file_to_arg);
    free(program_name);
    free(cmd);
    free(args);
    free(first_args);
    
    for (i = 0; i < pathitems; i++) {
        free(path[i]);
    }
    
    free(path);
    return 0;
}

