#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// fixed
//
#define FIGHTING_AGE_MAX 50
#define FIGHTING_AGE_MIN 20
#define WIFE_MIN_YEARS_SINCE_CHILD 2
#define MALE_ODDS 0.5 // 50%

// var

#define POP_START_COUNT 50
#define POP_START_MEAN_AGE 25
#define POP_START_STDDEV_AGE 2

#define MIN_MARRIED_AGE 20

#define MAX_WIVES 5
#define ANN_ADD_WIFE_ODDS 0.0046 // 10% total for 1

#define ANN_CHILD_BIRTH_ODDS  0.55 // 50%
#define BIRTH_DEATH_ODDS 0.05 // 5%
#define ANN_CHILD_DEATH_ODDS 0.0016 // 5% total
#define ANN_ADULT_DEATH_ODDS 0.0006 // 2% total

// technical

#define ALLOC_SIZE (10 * 1024 * 1024) // 10mb

// pop

struct Man {
	uint8_t age;
	uint8_t married_age;
	int8_t wives[5];
};

int free_list[10 * 1024]; // 10kb
int free_top = -1;
//int pop_count = POP_START_COUNT;
int last_valid_index = POP_START_COUNT - 1;

int add_man(struct Man pop[], uint8_t married_age) {
	int i;
	if (last_valid_index + 1 >= (ALLOC_SIZE / sizeof(struct Man))) {
		printf("%d", ALLOC_SIZE / sizeof(struct Man));
		printf("%d", last_valid_index);
		exit(0);
		printf("ERROR: Pop exceeds alloc mem\n");
		return -1;
	}
	if (free_top >= 0) {
		i = free_list[free_top--];
	} else {
		i = last_valid_index++;
	}

	if (pop[i].married_age != 0) {
		printf("WARNING: Overwriting person at index %d\n", i);
	}

	pop[i].age = 0;
	pop[i].married_age = married_age;
	for (int j = 0; j < MAX_WIVES; j++) {
		pop[i].wives[j] = -1;
	}

	if (i > last_valid_index) { 
		last_valid_index = i;
	}

	return i;
}

void rm_man(int i, struct Man pop[]) {
	memset(&pop[i], 0, sizeof(struct Man));
	free_list[++free_top] = i;
	if (i == last_valid_index) {
		while (last_valid_index >= 0 && pop[last_valid_index].married_age == 0) {
			last_valid_index--;
		}
	}
	if (last_valid_index < 0) last_valid_index = 0;
}

// utils

double gen_normal(double mean, double stddev) {
	double u1 = ((double) rand() / RAND_MAX);
	double u2 = ((double) rand() / RAND_MAX);
	double z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2); // normal
	return mean + stddev * z0;
}

int main() {
	srand(time(NULL));

	// init precomp married ages

	uint8_t precomp_married_ages[100];
	for (int i = 0; i < 100; i++) {
		double married_age = gen_normal(25, 5);
		if (married_age < 20) married_age = 20;
		precomp_married_ages[i] = (uint8_t) round(married_age);
	}

	// init pop
	
	struct Man *pop = (struct Man *)malloc(ALLOC_SIZE);
	if (!pop) {
		printf("Malloc failed\n");
		return 1;
	}
	memset(pop, 0, ALLOC_SIZE);
	
	for (int i = 0; i < POP_START_COUNT; i++) {
		pop[i].age = (uint8_t) round(gen_normal(POP_START_MEAN_AGE, POP_START_STDDEV_AGE));
		pop[i].married_age = precomp_married_ages[i % 100];
		for (int j = 0; j < MAX_WIVES; j++) {
			pop[i].wives[j] = -1;
		}
		if (pop[i].age >= pop[i].married_age) {
			pop[i].wives[0] = WIFE_MIN_YEARS_SINCE_CHILD;
		}
	}

	//printf("Size of struct Man: %zu bytes\n", sizeof(struct Man));
	//printf("Allocated space for %lu men (~10MB)\n", ALLOC_SIZE / sizeof(struct Man));

	// run sim to 1513 B.C.E. (215y)
	
	for (int year = 0; year < 215; year++) {
		// printf("\n --- year %d --- \n", 1728 - year+1);
		for (int i = 0; i < last_valid_index; i++) { // while
			if (pop[i].married_age == 0) { continue; } // should be filled
			
			pop[i].age += 1;
			if (pop[i].age > FIGHTING_AGE_MAX) { rm_man(i, pop); continue; }

			// marriage
			if (pop[i].age == pop[i].married_age) {
				pop[i].wives[0] = WIFE_MIN_YEARS_SINCE_CHILD;
			} else if ((rand() / (double)RAND_MAX) < ANN_ADD_WIFE_ODDS) {
				for (int j = 0; j < MAX_WIVES; j++) {
					if (pop[i].wives[j] == -1) {  // new spot available
						pop[i].wives[j] = WIFE_MIN_YEARS_SINCE_CHILD;
						break;
					}
				}
			}

			// wife and birth
			for (int j = 0; j < MAX_WIVES; j++) {
				if (pop[i].wives[j] == -1) { break; }
				pop[i].wives[j] += 1;

				if (pop[i].wives[j] < WIFE_MIN_YEARS_SINCE_CHILD) { continue; }
				if ((rand() / (double)RAND_MAX) <  ANN_CHILD_BIRTH_ODDS) {
					pop[i].wives[j] = 0;
					if (
						(rand() / (double)RAND_MAX) < MALE_ODDS &&
						(rand() / (double)RAND_MAX) > BIRTH_DEATH_ODDS
					) {
						int married_age = precomp_married_ages[i % 100];
						int a = add_man(pop, married_age);	
					}
				}
			}

			if (pop[i].age < 18 && (rand() / (double)RAND_MAX) < ANN_CHILD_DEATH_ODDS) {
				rm_man(i, pop); continue;
			} else if ((rand() / (double)RAND_MAX) < ANN_ADULT_DEATH_ODDS) {
				rm_man(i, pop); continue;
			}

			// print
			/*	
			printf("%d - Age: %d, Married Age: %d, Wives: [", i + 1, pop[i].age, pop[i].married_age);
			for (int j = 0; j < MAX_WIVES; j++) {
				printf("%d%s", pop[i].wives[j], (j < MAX_WIVES - 1) ? ", " : "");
			}
			printf("]\n");
			*/
		}
	}
	
	int count = 0;
	for (int i = 0; i < last_valid_index; i++) { // while
		if (pop[i].married_age == 0) { continue; } // should always be filled
		if (pop[i].age >= 20) { count += 1; }
	}
	printf("count: %d\n", count);

	return 0;
} 
