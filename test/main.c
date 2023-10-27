#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define BUF_OUTPUT_SIZE 128

struct TestCase
{
    char **input;
    char expected_output[BUF_OUTPUT_SIZE];
    char actual_output[BUF_OUTPUT_SIZE];
};

#define NUM_TESTS 7 // The number of test cases.

struct TestCase *create_test_cases(void);

void test_case_1(struct TestCase *test_case);

void test_case_2(struct TestCase *test_case);

void test_case_3(struct TestCase *test_case);

void test_case_4(struct TestCase *test_case);

void test_case_5(struct TestCase *test_case);

void test_case_6(struct TestCase *test_case);

void test_case_7(struct TestCase *test_case);

char **assemble_input(size_t num_args, ...);

void run_tests(struct TestCase *test_cases);

void run_test(struct TestCase *test_case, int pipe_r);

void report(struct TestCase *test_cases);

void free_test_cases(struct TestCase *test_cases);

int main(void)
{
    struct TestCase *test_cases = create_test_cases();
    if (!test_cases)
    {
        return 1;
    }
    
    run_tests(test_cases);
    
    report(test_cases);
    
    free_test_cases(test_cases);
    
    return 0;
}


struct TestCase *create_test_cases(void)
{
    struct TestCase *test_cases;
    
    test_cases = malloc(sizeof(struct TestCase) * NUM_TESTS);
    
    int offset = 0;
    test_case_1(test_cases + offset++);
    test_case_2(test_cases + offset++);
    test_case_3(test_cases + offset++);
    test_case_4(test_cases + offset++);
    test_case_5(test_cases + offset++);
    test_case_6(test_cases + offset++);
    test_case_7(test_cases + offset++);
    
    return test_cases;
}

void test_case_1(struct TestCase *test_case)
{
    long ans = (6000 - 321) / 11;
    test_case->input = assemble_input(1, "(6000 - 321) / 11");
    sprintf(test_case->expected_output, "%ld", ans);
}

void test_case_2(struct TestCase *test_case)
{
    long ans = 2 * 3 * 4 * 5;
    test_case->input = assemble_input(5, "2", "*", "3", "*", "4");
    sprintf(test_case->expected_output, "%ld", ans);
}

void test_case_3(struct TestCase *test_case)
{
    double ans = (6000.0 - 321.0) / 11;
    test_case->input = assemble_input(1, "(6000.0 - 321.0) / 11");
    sprintf(test_case->expected_output, "%lf", ans);
}

void test_case_4(struct TestCase *test_case)
{
    long ans = 0;
    test_case->input = assemble_input(1, "0");
    sprintf(test_case->expected_output, "%ld", ans);
}

void test_case_5(struct TestCase *test_case)
{
    long ans = -69 + 420;
    test_case->input = assemble_input(2, "-69", " +420");
    sprintf(test_case->expected_output, "%ld", ans);
}

void test_case_6(struct TestCase *test_case)
{
    long ans = (42 * (62 + 20));
    test_case->input = assemble_input(1, "(42 * (62 + 20)");
    sprintf(test_case->expected_output, "%ld", ans);
}

void test_case_7(struct TestCase *test_case)
{
    double ans = ((-20 - 2) / 2.5) + 14;
    test_case->input = assemble_input(2, "((-20 - 2) / 2.5) + 14");
    sprintf(test_case->expected_output, "%lf", ans);
}

char **assemble_input(size_t num_args, ...)
{
    char **input_array = malloc(sizeof(char *) * num_args);
    if (!input_array)
    {
        return NULL;
    }
    
    va_list args;
    va_start(args, num_args);
    
    for (size_t i = 0; i < num_args; ++i)
    {
        *(input_array + i) = va_arg(args, char *);
    }
    
    return input_array;
}

#define READ 0
#define WRITE 1

void run_tests(struct TestCase *test_cases)
{
    int ret_val;
    int res_pipe[2];
    ret_val = pipe(res_pipe);
    if (ret_val == -1)
    {
        // Error.
        return;
    }
    
    int stdout_std = dup(STDOUT_FILENO);
    dup2(res_pipe[WRITE], STDOUT_FILENO);
    for (int offset = 0; offset < NUM_TESTS; ++offset)
    {
        run_test(test_cases + offset, res_pipe[READ]);
    }
    dup2(stdout_std, STDOUT_FILENO);
}

void run_test(struct TestCase *test_case, int pipe_r)
{
    int   ret_val;
    pid_t id;
    
    id = fork();
    if (id == 0) // Child.
    {
        ret_val = execv("../math", test_case->input);
        if (ret_val == -1)
        {
            return;
        }
    } else // Parent.
    {
        ssize_t bytes_read;
        
        waitpid(id, NULL, 0);
        bytes_read = read(pipe_r, test_case->actual_output, BUF_OUTPUT_SIZE - 1);
        test_case->expected_output[bytes_read] = '\0';
    }
}

void report(struct TestCase *test_cases)
{
    bool all_passed;
    int  num_passed;
    
    all_passed = true;
    num_passed = NUM_TESTS;
    for (int offset = 0; offset < NUM_TESTS; ++offset)
    {
        if (!strcmp((test_cases + offset)->actual_output, (test_cases + offset)->expected_output))
        {
            all_passed = false;
            --num_passed;
            printf("[!] Test %d failed:\n\tExpected output: %s\n\tActual output: %s\n",
                   offset + 1, (test_cases + offset)->expected_output, (test_cases + offset)->actual_output);
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
        free((test_cases + offset)->input);
    }
    free(test_cases);
}
