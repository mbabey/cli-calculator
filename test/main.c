#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "testcases.h"

/**
 * Create TestCase structs testing program.
 * @return array of TestCases
 */
static struct TestCase *create_test_cases(void);

/**
 * Duplicate stdout to a pipe, then run each test in test_cases. Repair stdout after all tests have run.
 * @param test_cases the test cases to run.
 * @return the result of forking the process
 */
static pid_t run_tests(struct TestCase *test_cases);

/**
 * Fork this process. In the child, call execv with the arguments in test_case.
 * In the parent, read the program output from the pipe and store it in actual output.
 * @param test_case the test to run
 * @param pipe_r the pipe from which to read the program result
 * @return the result of forking the process
 */
static pid_t run_test(struct TestCase *test_case, int pipe_r);

/**
 * Report the results of the tests. Failed tests will be reported in detail.
 * @param test_cases the array of TestCases
 */
static void report(struct TestCase *test_cases);

/**
 * Free the array of TestCases.
 * @param test_cases the array of TestCases
 */
static void free_test_cases(struct TestCase *test_cases);

int main(void)
{
    struct TestCase *test_cases = create_test_cases();
    if (!test_cases)
    {
        return 1;
    }
    
    pid_t id = run_tests(test_cases);
    
    if (id > 0)
    {
        report(test_cases);
    }
    
    free_test_cases(test_cases);
    
    if (id == -1)
    {
        return 1;
    }
    
    return 0;
}

struct TestCase *create_test_cases(void)
{
    struct TestCase *test_cases;
    
    test_cases = malloc(sizeof(struct TestCase) * NUM_TESTS);
    
    int offset = 0;
    test_case_0(test_cases + offset++);
    test_case_1(test_cases + offset++);
    test_case_2(test_cases + offset++);
    test_case_3(test_cases + offset++);
    test_case_4(test_cases + offset++);
    test_case_5(test_cases + offset++);
    test_case_6(test_cases + offset++);
    test_case_7(test_cases + offset++);
    test_case_8(test_cases + offset++);
    test_case_9(test_cases + offset++);
    test_case_10(test_cases + offset++);
    test_case_11(test_cases + offset++);
    test_case_12(test_cases + offset++);
    test_case_13(test_cases + offset++);
    test_case_14(test_cases + offset++);
    test_case_15(test_cases + offset++);
    test_case_16(test_cases + offset++);

    return test_cases;
}

#define READ 0
#define WRITE 1

pid_t run_tests(struct TestCase *test_cases)
{
    int ret_val;
    int res_pipe[2];
    
    ret_val = pipe(res_pipe);
    if (ret_val == -1)
    {
        return 0;
    }
    
    pid_t id = 1;
    
    int stdout_std = dup(STDOUT_FILENO);
    dup2(res_pipe[WRITE], STDOUT_FILENO);
    for (int offset = 0; id > 0 && offset < NUM_TESTS; ++offset)
    {
        id = run_test(test_cases + offset, res_pipe[READ]);
    }
    dup2(stdout_std, STDOUT_FILENO);
    close(res_pipe[READ]);
    close(res_pipe[WRITE]);
    
    return id;
}

pid_t run_test(struct TestCase *test_case, int pipe_r)
{
    int   ret_val;
    pid_t id;
    
    id = fork();
    if (id == 0) // Child.
    {
        ret_val = execv(MATH_PROGRAM_FILENAME, test_case->input);
        if (ret_val == -1)
        {
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
            return -1;
        }
    } else // Parent.
    {
        ssize_t bytes_read;
        int     stat_val;
        
        waitpid(id, &stat_val, 0);
        if (WIFEXITED(stat_val) && WEXITSTATUS(stat_val) == 0)
        {
            bytes_read = read(pipe_r, test_case->actual_output, BUF_OUTPUT_SIZE - 1);
            test_case->actual_output[bytes_read] = '\0';
        } else
        {
            sprintf(test_case->actual_output, "Error occurred during execution. Exit code: %d\n", stat_val);
        }
    }
    return id;
}

void report(struct TestCase *test_cases)
{
    bool all_passed;
    int  num_passed;
    
    all_passed = true;
    num_passed = NUM_TESTS;
    for (int offset = 0; offset < NUM_TESTS; ++offset)
    {
        if (strcmp((test_cases + offset)->actual_output, (test_cases + offset)->expected_output) != 0)
        {
            all_passed = false;
            --num_passed;
            printf("[!] Test %d failed:\n\tInput: [%s", offset, *((test_cases + offset)->input + 1));
            for (size_t i = 2; i <= (test_cases + offset)->input_count; ++i)
            {
                printf(", %s", *((test_cases + offset)->input + i));
            }
            printf("]\n\tExpected output: %s\tActual output: %s",
                   (test_cases + offset)->expected_output, (test_cases + offset)->actual_output);
        } else
        {
            printf("[:] Test %d passed.\n", offset);
        }
    }
    
    if (all_passed)
    {
        printf("All tests passed.\n");
    } else
    {
        printf("%d/%d tests passed.\n", num_passed, NUM_TESTS);
    }
}

void free_test_cases(struct TestCase *test_cases)
{
    for (int offset = NUM_TESTS - 1; offset >= 0; --offset)
    {
        free(*((test_cases + offset)->input));
        free((test_cases + offset)->input);
    }
    free(test_cases);
}
