#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "munit.h"
#include "ex_sim.h"


static MunitResult test_add_male(const MunitParameter params[], void* data) {
    (void)params; 
    (void)data;

    struct Pop pop;
    memset(pop.items, -1, sizeof(pop.items));
    pop.items_top = -1;
    pop.free_top = -1;

    int m1 = add_male(&pop, 25);
    munit_assert_int(m1, ==, 0);
    munit_assert_int(pop.items[0].age, ==, 0);
    munit_assert_int(pop.items[0].married_age, ==, 25);

    int m2 = add_male(&pop, 30);
    munit_assert_int(m2, ==, 1);
    munit_assert_int(pop.items[1].age, ==, 0);
    munit_assert_int(pop.items[1].married_age, ==, 30);

    return MUNIT_OK;
}

static MunitResult test_rm_male(const MunitParameter params[], void* data) {
    (void)params; 
    (void)data;

    struct Pop pop;
    memset(pop.items, -1, sizeof(pop.items));
    pop.items_top = -1;
    pop.free_top = -1;

    add_male(&pop, 25);
    int m2 = add_male(&pop, 30);
    add_male(&pop, 35);

    rm_male(&pop, m2);
    munit_assert_int(pop.items[m2].age, ==, -1);

    int m4 = add_male(&pop, 40);
    munit_assert_int(m4, ==, m2);  // m2 should be reused

    return MUNIT_OK;
}

static MunitResult test_add_male_max_capacity(const MunitParameter params[], void* data) {
    (void)params;
    (void)data;

    struct Pop pop;
    memset(pop.items, -1, sizeof(pop.items));
    pop.items_top = -1;
    pop.free_top = -1;

    // Add males until we hit the maximum population
    for (int i = 0; i < POP_MAX; i++) {
        int res = add_male(&pop, 25);
        munit_assert_int(res, ==, i);
        munit_assert_int(pop.items[i].age, ==, 0);
        munit_assert_int(pop.items[i].married_age, ==, 25);
    }

    // Try adding one more male (should fail and exit)
    int res = add_male(&pop, 30);
    if (res == -1) {
        return MUNIT_OK;
    }

    return MUNIT_FAIL;
}

static MunitResult test_add_male_and_rm_all(const MunitParameter params[], void* data) {
    (void)params;
    (void)data;

    struct Pop pop;
    memset(pop.items, -1, sizeof(pop.items));
    pop.items_top = -1;
    pop.free_top = -1;

    // Add multiple males
    int m1 = add_male(&pop, 25);
    int m2 = add_male(&pop, 30);
    int m3 = add_male(&pop, 35);

    // Remove all males one by one
    rm_male(&pop, m1);
    rm_male(&pop, m2);
    rm_male(&pop, m3);

    // Verify all males are removed
    munit_assert_int(pop.items[m1].age, ==, -1);
    munit_assert_int(pop.items[m2].age, ==, -1);
    munit_assert_int(pop.items[m3].age, ==, -1);

    // Now, try to add new males, should reuse the freed indices
    int m4 = add_male(&pop, 40);
    munit_assert_int(m4, ==, m3);  // m1 should be reused
    munit_assert_int(pop.items[m4].age, ==, 0);
    munit_assert_int(pop.items[m4].married_age, ==, 40);

    int m5 = add_male(&pop, 45);
    munit_assert_int(m5, ==, m2);  // m2 should be reused
    munit_assert_int(pop.items[m5].age, ==, 0);
    munit_assert_int(pop.items[m5].married_age, ==, 45);

    int m6 = add_male(&pop, 50);
    munit_assert_int(m6, ==, m1);  // m3 should be reused
    munit_assert_int(pop.items[m6].age, ==, 0);
    munit_assert_int(pop.items[m6].married_age, ==, 50);

    return MUNIT_OK;
}

static MunitResult test_remove_last_male(const MunitParameter params[], void* data) {
    (void)params;
    (void)data;

    struct Pop pop;
    memset(pop.items, -1, sizeof(pop.items));
    pop.items_top = -1;
    pop.free_top = -1;

    // Add multiple males
    int m1 = add_male(&pop, 25);
    int m2 = add_male(&pop, 30);
    int m3 = add_male(&pop, 35);

    // Remove the last male (m3)
    rm_male(&pop, m3);
    munit_assert_int(pop.items[m3].age, ==, -1);  // m3 should be removed

    // Check the remaining males
    munit_assert_int(pop.items[m1].age, ==, 0);
    munit_assert_int(pop.items[m2].age, ==, 0);
    munit_assert_int(pop.items[m1].married_age, ==, 25);
    munit_assert_int(pop.items[m2].married_age, ==, 30);

    return MUNIT_OK;
}

static MunitResult test_rm_male_invalid_index(const MunitParameter params[], void* data) {
    (void)params;
    (void)data;

    struct Pop pop;
    memset(pop.items, -1, sizeof(pop.items));
    pop.items_top = -1;
    pop.free_top = -1;

    // Add a few males
    int m1 = add_male(&pop, 25);
    int m2 = add_male(&pop, 30);
    
    // Attempt to remove a male with an invalid index
    rm_male(&pop, -1);  // Invalid index
    rm_male(&pop, POP_MAX);  // Invalid index, out of bounds
    munit_assert_int(pop.items[m1].age, ==, 0);  // m1 should not be affected
    munit_assert_int(pop.items[m2].age, ==, 0);  // m2 should not be affected

    return MUNIT_OK;
}

static MunitTest tests[] = {
    { "/test_add_male", test_add_male, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/test_rm_male", test_rm_male, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/test_add_male_max_capacity", test_add_male_max_capacity, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/test_add_male_and_rm_all", test_add_male_and_rm_all, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/test_remove_last_male", test_remove_last_male, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/test_rm_male_invalid_index", test_rm_male_invalid_index, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite suite = {
    "/pop_tests", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char* argv[]) {
    return munit_suite_main(&suite, NULL, argc, argv);
}
