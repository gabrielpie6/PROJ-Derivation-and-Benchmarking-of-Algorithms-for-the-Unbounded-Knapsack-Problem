#ifndef UKPSOLVERS_H
#define UKPSOLVERS_H

typedef unsigned int Integer;
typedef long double Float;

typedef struct StProblem {
    Integer * v;
    Integer * w;
    Integer n;
    Integer c;
} * Problem;

Problem newProblem(Integer * v, Integer * w, Integer n, Integer c);
void    destroyProblem(Problem UKP);
Problem ReadProblem (char * filename);

unsigned long long int getRecursiveCalls();
void resetRecursiveCalls();

Integer FREC     (Problem UKP, Integer t);
Integer FPD      (Problem UKP, Integer t);
Integer GBACKREC (Problem UKP, Integer t, Integer k);
Integer GBACKPD  (Problem UKP, Integer t, Integer k);
Integer GFORWREC (Problem UKP, Integer t, Integer j);
Integer GFORWPD  (Problem UKP, Integer t, Integer j);
Integer BB2      (Problem UKP, Integer t, Integer j, Integer V);

#endif