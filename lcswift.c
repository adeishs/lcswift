#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <inttypes.h>

static uintmax_t get_width(void)
{
    static uintmax_t width = 0;
    static int done = 0;
    static uintmax_t n = UINTMAX_MAX;

    if (done) {
        return width;
    }

    while (n > 0) {
        ++width;
        n >>= 1;
    }
    done = 1;
    return width;
}

static size_t count_bits(uintmax_t v)
{
    size_t n = 0;

    for (n = 0; v; ++n) {
        v &= v - 1;
    }
    return n;
}

size_t lcs_len(const char *a, const char *b)
{
    size_t a_len = strlen(a);
    size_t b_len = strlen(b);
    size_t a_lower = 0;
    size_t a_upper = a_len;
    size_t b_lower = 0;
    size_t b_upper = b_len;
    const uintmax_t width = get_width();
    uintmax_t positions[UCHAR_MAX + 1] = {0};
    uintmax_t i = 0;
    uintmax_t v = ULLONG_MAX;
    uintmax_t p = 0;
    uintmax_t u = 0;

    if (a_len == 0 || b_len == 0) {
        return 0;
    }
    --a_upper;
    --b_upper;

    /* find the lower bound of difference */
    while (a_lower <= a_upper
           && b_lower <= b_upper
           && a[a_lower] == b[b_lower]) {
        ++a_lower;
        ++b_lower;
    }

    /* find the upper bound of difference */
    while (a_lower <= a_upper
           && b_lower <= b_upper
           && a[a_upper] == b[b_upper]) {
        --a_upper;
        --b_upper;
    }

    /* not supported yet */
    if (a_upper >= width) {
        return 0;
    }

    for (i = a_lower; i <= a_upper; ++i) {
        positions[(unsigned char)a[i]] |= 1 << (i % width);
    }

    for (i = b_lower; i <= b_upper; ++i) {
        p = positions[(unsigned char)b[i]];
        u = v & p;
        v = (v + u) | (v - u);
    }

    return a_lower + count_bits(~v) + a_len - 1 - a_upper;
}

struct test_case {
    char *a;
    char *b;
    size_t expected_value;
};

int main(void)
{
    int ret_val = EXIT_SUCCESS;
    struct test_case test_cases[] = {
        { "abcdef", "abcdef", 6 },
        { "aaaaa", "bbbbb", 0 },
        { "xabczdefy", "def", 3 }
    };
    size_t num_of_cases = sizeof test_cases / sizeof *test_cases;
    size_t success_count = 0;
    size_t failure_count = 0;
    size_t i = 0;

    for (; i < num_of_cases; ++i) {
        struct test_case *t = test_cases + i;
        size_t actual_value = lcs_len(t->a, t->b);

        if (t->expected_value == actual_value) {
            ++success_count;
        } else {
            ++failure_count;
            fprintf(stderr, "Not OK. Case %zu: Expected: %zu, Actual: %zu\n",
                    i, t->expected_value, actual_value);
        }
    }
    printf("%zu/%zu OK\n", success_count, num_of_cases);
    if (failure_count > 0) {
        printf("%zu/%zu OK\n", failure_count, num_of_cases);
        ret_val = EXIT_FAILURE;
    }

    return ret_val;
}
