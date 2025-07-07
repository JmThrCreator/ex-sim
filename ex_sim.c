#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "ex_sim.h"

// pop

int add_male(struct Pop *pop, int8_t married_age) {
	int i;
	if (pop->items_top + 1 >= POP_MAX) {
		return -1;
	} else if (pop->free_top >= 0) {
		i = pop->free_list[pop->free_top--];
	} else {
		i = ++pop->items_top;
	}

	if (pop->items[i].age != -1) {
		printf("WARNING: Overwriting male at %d\n", i);
	}
	pop->items[i].age = 0;
	pop->items[i].married_age = married_age;
	pop->items[i].sons = 0;
	return i;
}

void rm_male(struct Pop *pop, int i) {
	if (i < 0 || i >= POP_MAX) {
		return;
	}
	memset(&pop->items[i], -1, sizeof(struct Male));
	pop->free_list[++pop->free_top] = i;
	if (i == pop->items_top) {
		while (pop->items_top > 0 && pop->items[pop->items_top].age == -1) {
			pop->items_top --;
		}
	}
}

// util

double gen_normal(double mean, double stddev) {
	double u1 = ((double) rand() / RAND_MAX);
	double u2 = ((double) rand() / RAND_MAX);
	double z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2); // normal
	return mean + stddev * z0;
}


#ifdef TESTING
#else
int main() {
	srand(time(NULL));

	// precomp married ages

	uint8_t precomp_married_ages[100];
	for (int i = 0; i < 100; i++) {
		double married_age = gen_normal(MARRIED_AGE_MEAN, MARRIED_AGE_STDDEV);
		if (married_age < 20) married_age = 20;
		precomp_married_ages[i] = (int8_t) round(married_age);
	}

	// init pop

	struct Pop *pop = malloc(sizeof(struct Pop));
	if (!pop) {
		perror("Failed to allocate struct Pop");
		return 1;
	}
	pop->items = malloc(sizeof(struct Male) * POP_MAX);
	if (!pop->items) {
		perror("Failed to allocate items array");
		free(pop);
		exit(EXIT_FAILURE);
	}
	pop->free_top = -1;
	pop->items_top = POP_START_COUNT - 1;
	memset(pop->items, -1, sizeof(struct Male) * POP_MAX); // Fixed sizeof

	for (int i = 0; i < POP_START_COUNT; i++) {
		pop->items[i].age = (int8_t) round(gen_normal(POP_START_MEAN_AGE, POP_START_STDDEV_AGE));
		pop->items[i].married_age = precomp_married_ages[i % 100];
		pop->items[i].sons = 0;
		if (pop->items[i].age >= pop->items[i].married_age) {
			pop->items[i].wives[0] = WIFE_MIN_YEARS_SINCE_CHILD;
		}
	}

	// run sim for 215 years
	
	for (int year = 0; year < 215; year++) {
		//int count = 0;
		int tmp_items_top = pop->items_top; // prevent loop updating new-born appended to end
		for (int i = 0; i < tmp_items_top; i++) {
			if (pop->items[i].age == -1) continue;
			
			pop->items[i].age++;
			if (pop->items[i].age > FIGHTING_AGE_MAX) { rm_male(pop, i); continue; }
			//count++;

			// marriage
			
			if (pop->items[i].age == pop->items[i].married_age) {
				pop->items[i].wives[0] = WIFE_MIN_YEARS_SINCE_CHILD;
			} else if ((rand() / (double)RAND_MAX) < ANN_ADD_WIFE_ODDS) {
				for (int j = 0; j < MAX_WIVES; j++) {
					if (pop->items[i].wives[j] == -1) {
						pop->items[i].wives[j] = WIFE_MIN_YEARS_SINCE_CHILD;
						break;
					}
				}
			}

			// wife and birth

			if (pop->items[i].age < MALE_FERTILITY_END_AGE) {
				for (int j = 0; j < MAX_WIVES; j++) {
					if(pop->items[i].wives[j] == -1) { break; }
					pop->items[i].wives[j] += 1;
					
					if (pop->items[i].wives[j] < WIFE_MIN_YEARS_SINCE_CHILD) { continue; }
					if ((rand() / (double)RAND_MAX) < ANN_CHILD_BIRTH_ODDS) {
						pop->items[i].wives[j] = 0;
						if (
							(rand() / (double)RAND_MAX) < MALE_ODDS &&
							(rand() / (double)RAND_MAX) > BIRTH_DEATH_ODDS
						) {
							int8_t married_age = precomp_married_ages[i % 100];
							int res = add_male(pop, married_age);
							if (res == -1) {
								printf("ERROR: Max population reached\n");
								return 1;
							}
							pop->items[i].sons++;
						}
					}
				}
			}


			if (pop->items[i].age < MALE_ADULT_AGE && (rand() / (double)RAND_MAX) < ANN_CHILD_DEATH_ODDS) {
				rm_male(pop, i); continue;
			} else if ((rand() / (double)RAND_MAX) < ANN_ADULT_DEATH_ODDS) {
				rm_male(pop, i); continue;
			}
		}
		//printf("Year %d: %d\n", year, count);
	}

	int total = 0;
	int fighting_males = 0;
	int total_sons = 0;
	int married_count = 0;
	int total_wives_if_married = 0;
	for (int i = 0; i < pop->items_top; i++) {
		if (pop->items[i].age == -1) { continue; }
		total ++;
		if (pop->items[i].married_age == 0) { continue; }
		if (pop->items[i].age >= 20) { fighting_males ++; }
		total_sons ++;
		if (pop->items[i].age < pop->items[i].married_age) { continue; }
		married_count ++;
		for (int j = 0; j < MAX_WIVES; j++) {
			if(pop->items[i].wives[j] == -1) { break; }
			total_wives_if_married ++;
		}
	}
	printf("Total males: %d\n", total);
	printf("Fighting males: %d\n", fighting_males);
	printf("Current avg sons: %f\n", (float)total_sons/fighting_males);
	printf("Current avg wives: %f\n", (float)total_wives_if_married/married_count);

	// Free allocated memory
	free(pop->items);
	free(pop);
}
#endif
