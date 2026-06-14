#include "ukpsolvers.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define max(a, b) (((a) > (b)) ? (a) : (b))

static unsigned long long int recursive_calls = 0;

unsigned long long int getRecursiveCalls()
{
    return recursive_calls;
}

void resetRecursiveCalls()
{
    recursive_calls = 0;
}




Problem newProblem(Integer * v, Integer * w, Integer n, Integer c)
{
    Problem ukp = malloc(sizeof(struct StProblem));
    ukp->v = v;
    ukp->w = w;
    ukp->n = n;
    ukp->c = c;
    return ukp;
}

void destroyProblem(Problem UKP)
{
    // Free UKP structure
    free(UKP);
}



Problem ReadProblem(char * filename) {
    FILE *f = fopen(filename, "r");
    if (!f) { perror("fopen"); exit(1); }

    Integer n, c;
    fscanf(f, " n: %d", &n);
    fscanf(f, " c: %d", &c);
    fscanf(f, " begin data");

    Integer *w = malloc(n * sizeof(Integer));
    Integer *v = malloc(n * sizeof(Integer));
    if (!w || !v) { perror("malloc"); exit(1); }

    for (Integer i = 0; i < n; i++)
        fscanf(f, " %d %d", &w[i], &v[i]);

    fclose(f);

    Problem p = newProblem(v, w, n, c);
    return p;
}






//
// SOLVERS
//
Integer FREC(Problem UKP, Integer t)
{
    recursive_calls++;

    Integer fp = 0;
    Integer f;
    if(t > 0)
    {
        for (Integer k = 1; k <= UKP->n; k++)
        {
            if (UKP->w[k-1] <= t)
            {
                f  = FREC(UKP, t - UKP->w[k-1]);
                fp = max(fp, f + UKP->v[k-1]);
            }
        }
    }
    return fp;
}

Integer _FPD(Problem UKP, Integer t, Integer * MEMORY)
{
    recursive_calls++;

    // STAGE 1
    if (MEMORY[t] != (Integer)-1)
        return MEMORY[t];

    // STAGE 2
    Integer fp = 0;
    Integer f;
    if(t > 0)
    {
        for (Integer k = 1; k <= UKP->n; k++)
        {
            if (UKP->w[k-1] <= t)
            {
                f  = _FPD(UKP, t - UKP->w[k-1], MEMORY);
                fp = max(fp, f + UKP->v[k-1]);
            }
        }
    }

    // Stage 3
    MEMORY[t] = fp;
    return fp;
}

Integer FPD(Problem UKP, Integer t)
{
    // Integer MEMORY[UKP->c+1];
    Integer * MEMORY = malloc(sizeof(Integer) * (UKP->c + 1));
    memset(MEMORY, -1, sizeof(Integer) * (UKP->c + 1));

    Integer res = _FPD(UKP, t, MEMORY);
    free(MEMORY);
    
    return res;
}

Integer GBACKREC(Problem UKP, Integer t, Integer k)
{
    recursive_calls++;

    Integer gp = 0;
    Integer g;
    if(k >= 1)
    {
        for (Integer s = 0; s <= t / UKP->w[k-1]; s++)
        {
            g  = GBACKREC(UKP, t - s * UKP->w[k-1], k - 1);
            gp = max(gp, g + s * UKP->v[k-1]);
        }
    }
    return gp;
}

Integer _GBACKPD(Problem UKP, Integer t, Integer k, Integer * MEMORY)
{
    recursive_calls++;

    // STAGE 1
    if (k == 0)
        return 0;
    if (MEMORY[t * UKP->n + k - 1] != (Integer)-1)
        return MEMORY[t * UKP->n + k - 1];

    // STAGE 2
    Integer gp = 0;
    Integer g;
    for (Integer s = 0; s <= t / UKP->w[k-1]; s++)
    {
        g  = _GBACKPD(UKP, t - s * UKP->w[k-1], k - 1, MEMORY);
        gp = max(gp, g + s * UKP->v[k-1]);
    }

    // Stage 3
    MEMORY[t * UKP->n + k - 1] = gp;
    return gp;
}

Integer GBACKPD (Problem UKP, Integer t, Integer k)
{
    // Integer MEMORY[(UKP->c+1) * (UKP->n)];
    Integer * MEMORY = malloc(sizeof(Integer) * ((UKP->c + 1) * UKP->n));
    memset(MEMORY, -1, sizeof(Integer) * ((UKP->c + 1) * UKP->n));

    Integer res = _GBACKPD(UKP, t, k, MEMORY);
    free(MEMORY);
    
    return res;
}

Integer GFORWREC(Problem UKP, Integer t, Integer j)
{
    recursive_calls++;

    Integer gp = 0;
    Integer g;
    if(j <= UKP->n)
    {
        for (Integer s = 0; s <= t / UKP->w[j-1]; s++)
        {
            g  = GFORWREC(UKP, t - s * UKP->w[j-1], j + 1);
            gp = max(gp, g + s * UKP->v[j-1]);
        }
    }
    return gp;
}

Integer _GFORWPD(Problem UKP, Integer t, Integer j, Integer * MEMORY)
{
    recursive_calls++;

    // STAGE 1
    if (j == UKP->n + 1)
        return 0;
    if (MEMORY[t * UKP->n + j - 1] != (Integer)-1)
        return MEMORY[t * UKP->n + j - 1];

    // STAGE 2
    Integer gp = 0;
    Integer g;
    for (Integer s = 0; s <= t / UKP->w[j-1]; s++)
    {
        g  = _GFORWPD(UKP, t - s * UKP->w[j-1], j + 1, MEMORY);
        gp = max(gp, g + s * UKP->v[j-1]);
    }

    // Stage 3
    MEMORY[t * UKP->n + j - 1] = gp;
    return gp;
}

Integer GFORWPD(Problem UKP, Integer t, Integer j)
{
    // Integer MEMORY[(UKP->c+1) * (UKP->n)];
    Integer * MEMORY = malloc(sizeof(Integer) * ((UKP->c + 1) * UKP->n));
    memset(MEMORY, -1, sizeof(Integer) * ((UKP->c + 1) * UKP->n));

    Integer res = _GFORWPD(UKP, t, j, MEMORY);
    free(MEMORY);

    return res;
}



//
// BRANCH AND BOUND
//
Integer _BB2(Problem UKP, Integer t, Integer j, Integer V)
{
    recursive_calls++;

    Integer gp = 0;
    Integer g, s, glim;
    Float factor;
    if (j <= UKP->n - 1)
    {
        Integer smax = t / UKP->w[j-1];
        for (Integer q = 0; q <= smax; q++)
        {
            s = smax - q;
            factor = (Float) UKP->v[j] / UKP->w[j];
            glim = s * UKP->v[j-1] + V + (Integer)(factor * (t - s * UKP->w[j-1]));
            if (gp < glim)
            {
                g = _BB2(UKP, t - s*UKP->w[j-1], j+1, V + s*UKP->v[j-1]);
                gp = max(gp, g);
            }
        }
    }
    else if(j == UKP->n)
    {
        gp = V + (t / UKP->w[j-1]) * UKP->v[j-1];
    }
    return gp;
}

typedef struct { Integer v, w; Float density;} BBPair;

int cmp(const void *a, const void *b) {
    Float da = ((const BBPair*)a)->density;
    Float db = ((const BBPair*)b)->density;
    if (da < db) return  1;
    if (da > db) return -1;
    return 0;
}

Integer BB2(Problem UKP, Integer t, Integer j, Integer V)
{
    BBPair p[UKP->n];
    for (Integer i = 0; i < UKP->n; i++)
    {
        p[i].v = UKP->v[i];
        p[i].w = UKP->w[i];
        p[i].density = (Float) UKP->v[i] / UKP->w[i];
    }

    // Sorting by higher density
    qsort(p, UKP->n, sizeof p[0], cmp);

    Integer v[UKP->n];
    Integer w[UKP->n];

    for (Integer i = 0; i < UKP->n; i++)
    {
        v[i] = p[i].v;
        w[i] = p[i].w;
    }

    Problem ukpBB = newProblem(v, w, UKP->n, UKP->c);

    Integer result = _BB2(ukpBB, t, j, V);

    destroyProblem(ukpBB);
    return result;
}