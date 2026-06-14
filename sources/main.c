#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

#include <unistd.h>
#include <time.h>

#include "ukpsolvers.h"

#define TimeInterval(start, end) ((end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9)

int remover_newline_final(FILE *fp) {
    if (fflush(fp) != 0) return -1;

    if (fseek(fp, 0, SEEK_END) != 0) return -1;
    long fim = ftell(fp);
    if (fim < 0) return -1;
    long novo = fim;

    if (fim >= 2) {
        fseek(fp, fim - 2, SEEK_SET);
        int c1 = fgetc(fp);
        int c2 = fgetc(fp);
        if (c1 == '\r' && c2 == '\n')  novo = fim - 2;  /* \r\n */
        else if (c2 == '\n')           novo = fim - 1;  /* \n   */
    } else if (fim == 1) {
        fseek(fp, 0, SEEK_SET);
        if (fgetc(fp) == '\n') novo = 0;
    }

    if (novo != fim) {
        fflush(fp);
        if (ftruncate(fileno(fp), novo) != 0) return -1;
    }
    fseek(fp, novo, SEEK_SET);
    return 0;
}

int main() {
    DIR *dir;
    struct dirent *ent;
    char path[200];
    char outpath[1000];
    char outpathfile[1200];

    char *classes[] = {"P1C", "P2C", "P3C", "M1C", "M2C", "M3C", "G1C", "G2C", "G3C"};
    char *base;
    int n_classes = sizeof(classes)/sizeof(classes[0]);

    Problem UKP;
    Integer result;

    // Metrics
    struct timespec t0, t1;
    double timeElapsedSeconds;

    for (int i = 0; i < n_classes; i++) {
        sprintf(path, "../classes/%s", classes[i]);
        sprintf(outpath, "../output/%s", classes[i]);
        
        // printf("%s\n", path);
        sprintf(outpath, "../output/results-%s.data", classes[i]);
        FILE *outFile = fopen(outpath, "w+");
        if (!outFile) {
            perror("Não foi possível criar o arquivo de saída\n-- Verifique se o diretório de saída [/output] existe.\n");
            return EXIT_FAILURE;
        }
        
        fprintf(outFile, "Instance   Time_FPD   Calls_FPD   MaxDepth_FPD   Time_GBACKPD   Calls_GBACKPD   MaxDepth_GBACKPD   Time_GFORWPD   Calls_GFORWPD   MaxDepth_GFORWPD   Time_BB2   Calls_BB2   MaxDepth_BB2   OptimalVaue\n");
        if ((dir = opendir(path)) != NULL) {
            while ((ent = readdir(dir)) != NULL) {
                if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                    char filepath[1000];
                    sprintf(filepath, "%s/%s", path, ent->d_name);
                    base = strtok(ent->d_name, ".");

                    printf("Calculating for: %s\n", filepath);
                    UKP = ReadProblem(filepath);
                    
                    // Get only the last 3 digits of the base name
                    base = base + strlen(base) - 3;
                    fprintf(outFile, "%s", base);
                    
                    resetRecursiveCalls();
                    
                    // result = FREC(UKP, UKP->c);
                    // printf("FREC:      %u\n", result);
                    
                    // ---
                    // FPD
                    clock_gettime(CLOCK_MONOTONIC, &t0);
                    result = FPD(UKP, UKP->c);
                    clock_gettime(CLOCK_MONOTONIC, &t1);
                    timeElapsedSeconds = TimeInterval(t0, t1);
                    fprintf(outFile, "   %.6E %.6E", timeElapsedSeconds, (double) getRecursiveCalls());
                    resetRecursiveCalls();
                    printf("FPD:       %u\n", result);
                    // ---

                    // result = GBACKREC(UKP, UKP->c, UKP->n);
                    // printf("GBACKREC:  %u\n", result);
                    


                    // ---
                    // GBACKPD
                    clock_gettime(CLOCK_MONOTONIC, &t0);
                    result = GBACKPD(UKP, UKP->c, UKP->n);
                    clock_gettime(CLOCK_MONOTONIC, &t1);
                    timeElapsedSeconds = TimeInterval(t0, t1);
                    fprintf(outFile, "   %.6E %.6E", timeElapsedSeconds, (double) getRecursiveCalls());
                    resetRecursiveCalls();
                    printf("GBACKPD:   %u\n", result);
                    
                    // ---
                    
                    // result = GFORWREC(UKP, UKP->c, 1);
                    // printf("GFORWREC:  %u\n", result);
                    
                    // ---
                    // GFORWPD
                    clock_gettime(CLOCK_MONOTONIC, &t0);
                    result = GFORWPD(UKP, UKP->c, 1);
                    clock_gettime(CLOCK_MONOTONIC, &t1);
                    timeElapsedSeconds = TimeInterval(t0, t1);
                    fprintf(outFile, "   %.6E %.6E", timeElapsedSeconds, (double) getRecursiveCalls());
                    resetRecursiveCalls();
                    printf("GFORWPD:   %u\n", result);
                    // ---
                    
                    
                    // ---
                    // BB2
                    clock_gettime(CLOCK_MONOTONIC, &t0);
                    result = BB2(UKP, UKP->c, 1, 0);
                    clock_gettime(CLOCK_MONOTONIC, &t1);
                    timeElapsedSeconds = TimeInterval(t0, t1);
                    fprintf(outFile, "   %.6E %.6E %u", timeElapsedSeconds, (double) getRecursiveCalls(), result);
                    resetRecursiveCalls();
                    printf("BB2:       %u\n", result);
                    // ---


                    fprintf(outFile, "\n");
                    free(UKP->v);
                    free(UKP->w);
                    destroyProblem(UKP);
                }
            }
            closedir(dir);
        } else {
            perror("Não foi possível abrir o diretório");
            return EXIT_FAILURE;
        }


        // Removing last \r\n or \n from outFile
        if (remover_newline_final(outFile) != 0) {
            perror("Não foi possível remover a nova linha final do arquivo de saída");
            fclose(outFile);
            return EXIT_FAILURE;
        }

        fclose(outFile);
    }

    return 0;
}