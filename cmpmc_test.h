#ifndef CMPMC_TEST_H
#define CMPMC_TEST_H

int __failed_tests = 0;
int __total_tests = 0;
#define test_cond(msg, cond)                                                   \
  do {                                                                         \
    __total_tests++;                                                           \
    fprintf(stdout, "test - %s: ", msg);                                       \
    if (cond)                                                                  \
      printf("OK\n");                                                          \
    else {                                                                     \
      printf("FAILED\n");                                                      \
      __failed_tests++;                                                        \
    }                                                                          \
  } while (0);

#define test_report()                                                          \
  do {                                                                         \
    printf("- %d tests\n- %d tests PASSED\n- %d tests FAILED\n",               \
           __total_tests, __total_tests - __failed_tests, __failed_tests);     \
  } while (0);

#endif
