#ifndef EX_SIM_H 
#define EX_SIM_H

#include <stdint.h>

// const

#define FIGHTING_AGE_MAX 50
#define FIGHTING_AGE_MIN 20
#define MALE_ODDS 0.5
#define MALE_ADULT_AGE 18
#define WIFE_MIN_YEARS_SINCE_CHILD 2

// var

#define POP_START_COUNT 50
#define POP_START_MEAN_AGE 15
#define POP_START_STDDEV_AGE 4

#define MIN_MARRIED_AGE 20
#define MARRIED_AGE_MEAN 22
#define MARRIED_AGE_STDDEV 3

#define MAX_WIVES 3
#define ANN_ADD_WIFE_ODDS 0.012 // ~10% will have 1 extra wife

#define MALE_FERTILITY_END_AGE 40

#define ANN_CHILD_BIRTH_ODDS  0.65
#define BIRTH_DEATH_ODDS 0.1
#define ANN_CHILD_DEATH_ODDS 0.006 // ~10% total
#define ANN_ADULT_DEATH_ODDS 0.001 // ~3% total

struct Male {
    int8_t age;
    int8_t married_age;
    int8_t sons;
    int8_t wives[MAX_WIVES];
};

#define POP_MAX 3000000 // ~24MB

struct Pop {
    struct Male *items;
    int items_top;
    int free_top;
    int free_list[10 * 1024]; // 10KB
};

int add_male(struct Pop *pop, int8_t married_age);
void rm_male(struct Pop *pop, int i);

#endif
