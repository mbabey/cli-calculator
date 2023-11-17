#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#define BUF_OUTPUT_SIZE 1024
#define MATH_PROGRAM_FILENAME "/Users/mud/Projects/cProjects/cmd-line-calculator/math"

/** The number of test cases. */
#define NUM_TESTS 17

/**
 * Stores test parameters. input is argv for the tested program. Expected output can
 * be checked against actual output.
 */
struct TestCase
{
    char   **input;
    size_t input_count;
    char   expected_output[BUF_OUTPUT_SIZE];
    char   actual_output[BUF_OUTPUT_SIZE];
};

/**
 * Create TestCase structs testing program.
 * @return array of TestCases
 */
struct TestCase *create_test_cases(void);

/**
 * Test order of operations.
 * @param test_case the TestCase to load
 */
void test_case_0(struct TestCase *test_case);

/**
 * Test parentheses.
 * @param test_case the TestCase to load
 */
void test_case_1(struct TestCase *test_case);

/**
 * Test multiple argv.
 * @param test_case the TestCase to load
 */
void test_case_2(struct TestCase *test_case);

/**
 * Test operation containing both doubles and longs.
 * @param test_case the TestCase to load
 */
void test_case_3(struct TestCase *test_case);

/**
 * Test 0.
 * @param test_case the TestCase to load
 */
void test_case_4(struct TestCase *test_case);

/**
 * Test negative number.
 * @param test_case the TestCase to load
 */
void test_case_5(struct TestCase *test_case);

/**
 * Test nested parentheses.
 * @param test_case the TestCase to load
 */
void test_case_6(struct TestCase *test_case);

/**
 * Test nested parentheses.
 * @param test_case the TestCase to load
 */
void test_case_7(struct TestCase *test_case);

/**
 * Test unmatched left parentheses.
 * @param test_case the TestCase to load
 */
void test_case_8(struct TestCase *test_case);

/**
 * Test unmatched right parentheses.
 * @param test_case the TestCase to load
 */
void test_case_9(struct TestCase *test_case);

/**
 * Test unmatched left unmatched primary. "4 /"
 * @param test_case the TestCase to load
 */
void test_case_10(struct TestCase *test_case);

/**
 * Test unmatched right unmatched primary. "/ 4"
 * @param test_case the TestCase to load
 */
void test_case_11(struct TestCase *test_case);

/**
 * Test help with "-h"
 * @param test_case the TestCase to load
 */
void test_case_12(struct TestCase *test_case);

/**
 * Test help with "-help"
 * @param test_case the TestCase to load
 */
void test_case_13(struct TestCase *test_case);

/**
 * Test help with less than 2 args.
 * @param test_case the TestCase to load
 */
void test_case_14(struct TestCase *test_case);

/**
 * Test right-aligned nested parenthesis
 * @param test_case the TestCase to load
 */
void test_case_15(struct TestCase *test_case);

/**
 * Test a lot of nested expressions.
 * @param test_case the TestCase to load
 */
void test_case_16(struct TestCase *test_case);

/**
 * Given a list of string arguments, create a argument vector. Prefixes the name of the
 * program being tested and suffixes NULL.
 * @param num_args the number of args
 * @param ... the args
 * @return pointer to the argument vector
 */
char **assemble_input(size_t num_args, ...);

/**
 * Duplicate stdout to a pipe, then run each test in test_cases. Repair stdout after all tests have run.
 * @param test_cases the test cases to run.
 * @return the result of forking the process
 */
pid_t run_tests(struct TestCase *test_cases);

/**
 * Fork this process. In the child, call execv with the arguments in test_case.
 * In the parent, read the program output from the pipe and store it in actual output.
 * @param test_case the test to run
 * @param pipe_r the pipe from which to read the program result
 * @return the result of forking the process
 */
pid_t run_test(struct TestCase *test_case, int pipe_r);

/**
 * Report the results of the tests. Failed tests will be reported in detail.
 * @param test_cases the array of TestCases
 */
void report(struct TestCase *test_cases);

/**
 * Free the array of TestCases.
 * @param test_cases the array of TestCases
 */
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

void test_case_0(struct TestCase *test_case)
{
    long ans = 1 + 2 - 3 * 4 / 5;
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "1 + 2 - 3 * 4 / 5");
    sprintf(test_case->expected_output, "%ld\n", ans);
}

void test_case_1(struct TestCase *test_case)
{
    long ans = (6000 - 321) / 11;
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "(6000 - 321) / 11");
    sprintf(test_case->expected_output, "%ld\n", ans);
}

void test_case_2(struct TestCase *test_case)
{
    long ans = 2 * 3 * 4 * 5;
    test_case->input_count = 7;
    test_case->input       = assemble_input(test_case->input_count, "2", "*", "3", "*", "4", "*", "5");
    sprintf(test_case->expected_output, "%ld\n", ans);
}

void test_case_3(struct TestCase *test_case)
{
    double ans = (6000.0 - 321.0) / 11;
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "(6000.0 - 321.0) / 11");
    sprintf(test_case->expected_output, "%lf\n", ans);
}

void test_case_4(struct TestCase *test_case)
{
    long ans = 0;
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "0");
    sprintf(test_case->expected_output, "%ld\n", ans);
}

void test_case_5(struct TestCase *test_case)
{
    long ans = -69 + 420;
    test_case->input_count = 2;
    test_case->input       = assemble_input(test_case->input_count, "-69", " +420");
    sprintf(test_case->expected_output, "%ld\n", ans);
}

void test_case_6(struct TestCase *test_case)
{
    long ans = (42 * (62 + 20));
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "(42 * (62 + 20))");
    sprintf(test_case->expected_output, "%ld\n", ans);
}

void test_case_7(struct TestCase *test_case)
{
    double ans = ((-20 - 2) / 2.5) + 14;
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "((-20 - 2) / 2.5) + 14");
    sprintf(test_case->expected_output, "%lf\n", ans);
}

void test_case_8(struct TestCase *test_case)
{
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "(20 * 8");
    sprintf(test_case->expected_output, "Unmatched \'(\' in expression. Use \'-h\' or \'-help\' for help.\n");
}

void test_case_9(struct TestCase *test_case)
{
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "20 * 8)");
    sprintf(test_case->expected_output, "Unmatched \')\' in expression. Use \'-h\' or \'-help\' for help.\n");
}


void test_case_10(struct TestCase *test_case)
{
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "4 /");
    sprintf(test_case->expected_output, "Incomplete expression. Use \'-h\' or \'-help\' for help.\n");
}

void test_case_11(struct TestCase *test_case)
{
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "/ 4");
    sprintf(test_case->expected_output, "Incomplete expression. Use \'-h\' or \'-help\' for help.\n");
}

#define COLOR_BOLD  "\033[1m"
#define COLOR_OFF   "\033[m"

void test_case_12(struct TestCase *test_case)
{
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "-h");
    sprintf(test_case->expected_output, COLOR_BOLD "NAME\n"
                                        "\tmath" COLOR_OFF " - command line calculator\n"
                                        COLOR_BOLD "\nSYNOPSIS\n" COLOR_OFF
                                        COLOR_BOLD "\tmath " COLOR_OFF "<" COLOR_BOLD "expression" COLOR_OFF ">\n"
                                        COLOR_BOLD "\nDESCRIPTION\n" COLOR_OFF
                                        "\tCalculates and displays the result of the mathematical expression <" COLOR_BOLD "expression" COLOR_OFF ">.\n"
                                        "\tInput operands can be whole numbers or decimal numbers. A single decimal number operand,\n"
                                        "\tregardless of the number of whole number operands, will make the result a decimal\n"
                                        "\tnumber.\n"
                                        "\n\t<" COLOR_BOLD "expression" COLOR_OFF "> may contain spaces. If <" COLOR_BOLD "expression" COLOR_OFF "> contains characters \'/\' or \'*\', it\n"
                                        "\tmust be wrapped in double quotes (eg: "COLOR_BOLD"\""COLOR_OFF"<"COLOR_BOLD"expression"COLOR_OFF">"COLOR_BOLD"\""COLOR_OFF").\n"
                                        "\n\tSupported operations are:"
                                        "\n\t\t" COLOR_BOLD "+" COLOR_OFF " - addition"
                                        "\n\t\t" COLOR_BOLD "-" COLOR_OFF " - subtraction"
                                        "\n\t\t" COLOR_BOLD "*" COLOR_OFF " - multiplication"
                                        "\n\t\t" COLOR_BOLD "/" COLOR_OFF " - division"
                                        "\n\t\t" COLOR_BOLD "^" COLOR_OFF " - exponentiation\n"
                                        COLOR_BOLD "\nEXAMPLES\n" COLOR_OFF
                                        "\tmath 3+4\n\tmath 3 + 4\n\tmath 3*4\n\tmath \"3 * 4\"\n\tmath \"((-20 - 2) * 4.5) / 11)\"\n");
}

void test_case_13(struct TestCase *test_case)
{
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "-help");
    sprintf(test_case->expected_output, COLOR_BOLD "NAME\n"
                                        "\tmath" COLOR_OFF " - command line calculator\n"
                                        COLOR_BOLD "\nSYNOPSIS\n" COLOR_OFF
                                        COLOR_BOLD "\tmath " COLOR_OFF "<" COLOR_BOLD "expression" COLOR_OFF ">\n"
                                        COLOR_BOLD "\nDESCRIPTION\n" COLOR_OFF
                                        "\tCalculates and displays the result of the mathematical expression <" COLOR_BOLD "expression" COLOR_OFF ">.\n"
                                        "\tInput operands can be whole numbers or decimal numbers. A single decimal number operand,\n"
                                        "\tregardless of the number of whole number operands, will make the result a decimal\n"
                                        "\tnumber.\n"
                                        "\n\t<" COLOR_BOLD "expression" COLOR_OFF "> may contain spaces. If <" COLOR_BOLD "expression" COLOR_OFF "> contains characters \'/\' or \'*\', it\n"
                                        "\tmust be wrapped in double quotes (eg: "COLOR_BOLD"\""COLOR_OFF"<"COLOR_BOLD"expression"COLOR_OFF">"COLOR_BOLD"\""COLOR_OFF").\n"
                                        "\n\tSupported operations are:"
                                        "\n\t\t" COLOR_BOLD "+" COLOR_OFF " - addition"
                                        "\n\t\t" COLOR_BOLD "-" COLOR_OFF " - subtraction"
                                        "\n\t\t" COLOR_BOLD "*" COLOR_OFF " - multiplication"
                                        "\n\t\t" COLOR_BOLD "/" COLOR_OFF " - division"
                                        "\n\t\t" COLOR_BOLD "^" COLOR_OFF " - exponentiation\n"
                                        COLOR_BOLD "\nEXAMPLES\n" COLOR_OFF
                                        "\tmath 3+4\n\tmath 3 + 4\n\tmath 3*4\n\tmath \"3 * 4\"\n\tmath \"((-20 - 2) * 4.5) / 11)\"\n");
}

void test_case_14(struct TestCase *test_case)
{
    test_case->input_count = 0;
    test_case->input       = assemble_input(test_case->input_count);
    sprintf(test_case->expected_output, COLOR_BOLD "NAME\n"
                                        "\tmath" COLOR_OFF " - command line calculator\n"
                                        COLOR_BOLD "\nSYNOPSIS\n" COLOR_OFF
                                        COLOR_BOLD "\tmath " COLOR_OFF "<" COLOR_BOLD "expression" COLOR_OFF ">\n"
                                        COLOR_BOLD "\nDESCRIPTION\n" COLOR_OFF
                                        "\tCalculates and displays the result of the mathematical expression <" COLOR_BOLD "expression" COLOR_OFF ">.\n"
                                        "\tInput operands can be whole numbers or decimal numbers. A single decimal number operand,\n"
                                        "\tregardless of the number of whole number operands, will make the result a decimal\n"
                                        "\tnumber.\n"
                                        "\n\t<" COLOR_BOLD "expression" COLOR_OFF "> may contain spaces. If <" COLOR_BOLD "expression" COLOR_OFF "> contains characters \'/\' or \'*\', it\n"
                                        "\tmust be wrapped in double quotes (eg: "COLOR_BOLD"\""COLOR_OFF"<"COLOR_BOLD"expression"COLOR_OFF">"COLOR_BOLD"\""COLOR_OFF").\n"
                                        "\n\tSupported operations are:"
                                        "\n\t\t" COLOR_BOLD "+" COLOR_OFF " - addition"
                                        "\n\t\t" COLOR_BOLD "-" COLOR_OFF " - subtraction"
                                        "\n\t\t" COLOR_BOLD "*" COLOR_OFF " - multiplication"
                                        "\n\t\t" COLOR_BOLD "/" COLOR_OFF " - division"
                                        "\n\t\t" COLOR_BOLD "^" COLOR_OFF " - exponentiation\n"
                                        COLOR_BOLD "\nEXAMPLES\n" COLOR_OFF
                                        "\tmath 3+4\n\tmath 3 + 4\n\tmath 3*4\n\tmath \"3 * 4\"\n\tmath \"((-20 - 2) * 4.5) / 11)\"\n");
}


void test_case_15(struct TestCase *test_case)
{
    double ans = (2.5 * (-20 - 2)) + 14;
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "(2.5 * (-20 - 2)) + 14");
    sprintf(test_case->expected_output, "%lf\n", ans);
}

void test_case_16(struct TestCase *test_case)
{
    double ans = ((88 - 87) * 1.25 * (-2 - 2) - (3 + 4 * (69 - -200) * 0.01) * 10);
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "((88 - 87) * 1.25 * (-2 - 2) - (3 + 4 * (69 - -200) * 0.01) * 10)");
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
    for (size_t i                 = 1; i <= num_args; ++i)
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
