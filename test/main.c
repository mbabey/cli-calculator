#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#define BUF_OUTPUT_SIZE 128
#define MATH_PROGRAM_FILENAME "/Users/mud/Projects/cProjects/cmd-line-calculator/math"

struct TestCase
{
    char **input;
    size_t input_count;
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

pid_t run_tests(struct TestCase *test_cases);

pid_t run_test(struct TestCase *test_case, int pipe_r);

void report(struct TestCase *test_cases);

void free_test_cases(struct TestCase *test_cases);

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
    test_case->input_count = 1;
    test_case->input = assemble_input(test_case->input_count, "(6000 - 321) / 11");
    sprintf(test_case->expected_output, "%ld\n", ans);
}

void test_case_2(struct TestCase *test_case)
{
    long ans = 2 * 3 * 4 * 5;
    test_case->input_count = 7;
    test_case->input = assemble_input(test_case->input_count, "2", "*", "3", "*", "4", "*", "5");
    sprintf(test_case->expected_output, "%ld\n", ans);
}

void test_case_3(struct TestCase *test_case)
{
    double ans = (6000.0 - 321.0) / 11;
    test_case->input_count = 1;
    test_case->input = assemble_input(test_case->input_count, "(6000.0 - 321.0) / 11");
    sprintf(test_case->expected_output, "%lf\n", ans);
}

void test_case_4(struct TestCase *test_case)
{
    long ans = 0;
    test_case->input_count = 1;
    test_case->input = assemble_input(test_case->input_count, "0");
    sprintf(test_case->expected_output, "%ld\n", ans);
}

void test_case_5(struct TestCase *test_case)
{
    long ans = -69 + 420;
    test_case->input_count = 2;
    test_case->input = assemble_input(test_case->input_count, "-69", " +420");
    sprintf(test_case->expected_output, "%ld\n", ans);
}

void test_case_6(struct TestCase *test_case)
{
    long ans = (42 * (62 + 20));
    test_case->input_count = 1;
    test_case->input = assemble_input(test_case->input_count, "(42 * (62 + 20))");
    sprintf(test_case->expected_output, "%ld\n", ans);
}

void test_case_7(struct TestCase *test_case)
{
    double ans = ((-20 - 2) / 2.5) + 14;
    test_case->input_count = 1;
    test_case->input = assemble_input(test_case->input_count, "((-20 - 2) / 2.5) + 14");
    sprintf(test_case->expected_output, "%lf\n", ans);
}

char **assemble_input(size_t num_args, ...)
{
    char **input_array = malloc(sizeof(char *) * (num_args + 2));
    if (!input_array)
    {
        return NULL;
    }
    
    va_list args;
    va_start(args, num_args);
    *(input_array) = strdup(MATH_PROGRAM_FILENAME);
    for (size_t i = 1; i <= num_args; ++i)
    {
        *(input_array + i) = va_arg(args, char *);
    }
    *(input_array + num_args + 1) = NULL;
    
    return input_array;
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
        // Error.
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
        ret_val = execv("/Users/mud/Projects/cProjects/cmd-line-calculator/math", test_case->input);
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
        fprintf(stderr, "%d\n", stat_val);
        if (WIFEXITED(stat_val) && WEXITSTATUS(stat_val) == 0)
        {
            bytes_read = read(pipe_r, test_case->actual_output, BUF_OUTPUT_SIZE - 1);
            test_case->actual_output[bytes_read] = '\0';
        } else
        {
            sprintf(test_case->actual_output, "Error occurred during execution.");
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
            printf("[!] Test %d failed:\n\tInput: [%s", offset + 1, *((test_cases + offset)->input + 1));
            for (size_t i = 2; i <= (test_cases + offset)->input_count; ++i)
            {
                printf(", %s", *((test_cases + offset)->input + i));
            }
            printf("]\n\tExpected output: %s\tActual output: %s",
                   (test_cases + offset)->expected_output, (test_cases + offset)->actual_output);
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
