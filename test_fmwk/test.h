#ifndef __TEST_H__
#define __TEST_H__

#define TEST_NAME_MAX_LEN 80

typedef struct test_arr_s {
    char test_name[TEST_NAME_MAX_LEN];
    void (*test_fn)(const char* test_name);
} test_arr_t;

#endif /*__TEST_H__*/
