#ifndef CMD_LINE_CALCULATOR_TEST_TESTCASES_H
#define CMD_LINE_CALCULATOR_TEST_TESTCASES_H

#include <stdarg.h>

#define BUF_OUTPUT_SIZE 1024

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

/** The number of test cases. */
#define NUM_TESTS 17

/**
 * Given a list of string arguments, create a argument vector. Prefixes the name of the
 * program being tested and suffixes NULL.
 * @param num_args the number of args
 * @param ... the args
 * @return pointer to the argument vector
 */
static char **assemble_input(size_t num_args, ...);

/**
 * Test order of operations.
 * @param test_case the TestCase to load
 */
static void test_case_0(struct TestCase *test_case)
{
    long ans = 1 + 2 - 3 * 4 / 5;
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "1 + 2 - 3 * 4 / 5");
    sprintf(test_case->expected_output, "%ld\n", ans);
}

/**
 * Test parentheses.
 * @param test_case the TestCase to load
 */
static void test_case_1(struct TestCase *test_case)
{
    long ans = (6000 - 321) / 11;
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "(6000 - 321) / 11");
    sprintf(test_case->expected_output, "%ld\n", ans);
}

/**
 * Test multiple argv.
 * @param test_case the TestCase to load
 */
static void test_case_2(struct TestCase *test_case)
{
    long ans = 2 * 3 * 4 * 5;
    test_case->input_count = 7;
    test_case->input       = assemble_input(test_case->input_count, "2", "*", "3", "*", "4", "*", "5");
    sprintf(test_case->expected_output, "%ld\n", ans);
}

/**
 * Test operation containing both doubles and longs.
 * @param test_case the TestCase to load
 */
static void test_case_3(struct TestCase *test_case)
{
    double ans = (6000.0 - 321.0) / 11;
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "(6000.0 - 321.0) / 11");
    sprintf(test_case->expected_output, "%lf\n", ans);
}

/**
 * Test 0.
 * @param test_case the TestCase to load
 */
static void test_case_4(struct TestCase *test_case)
{
    long ans = 0;
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "0");
    sprintf(test_case->expected_output, "%ld\n", ans);
}

/**
 * Test negative number.
 * @param test_case the TestCase to load
 */
static void test_case_5(struct TestCase *test_case)
{
    long ans = -69 + 420;
    test_case->input_count = 2;
    test_case->input       = assemble_input(test_case->input_count, "-69", " +420");
    sprintf(test_case->expected_output, "%ld\n", ans);
}

/**
 * Test nested parentheses.
 * @param test_case the TestCase to load
 */
static void test_case_6(struct TestCase *test_case)
{
    long ans = (42 * (62 + 20));
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "(42 * (62 + 20))");
    sprintf(test_case->expected_output, "%ld\n", ans);
}

/**
 * Test nested parentheses.
 * @param test_case the TestCase to load
 */
static void test_case_7(struct TestCase *test_case)
{
    double ans = ((-20 - 2) / 2.5) + 14;
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "((-20 - 2) / 2.5) + 14");
    sprintf(test_case->expected_output, "%lf\n", ans);
}

/**
 * Test unmatched left parentheses.
 * @param test_case the TestCase to load
 */
static void test_case_8(struct TestCase *test_case)
{
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "(20 * 8");
    sprintf(test_case->expected_output, "Unmatched \'(\' in expression. Use 'math -h' or 'math -help' for help.\n");
}

/**
 * Test unmatched right parentheses.
 * @param test_case the TestCase to load
 */
static void test_case_9(struct TestCase *test_case)
{
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "20 * 8)");
    sprintf(test_case->expected_output, "Unmatched \')\' in expression. Use 'math -h' or 'math -help' for help.\n");
}

/**
 * Test unmatched left unmatched primary. "4 /"
 * @param test_case the TestCase to load
 */
static void test_case_10(struct TestCase *test_case)
{
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "4 /");
    sprintf(test_case->expected_output, "Incomplete expression. Use 'math -h' or 'math -help' for help.\n");
}

/**
 * Test unmatched right unmatched primary. "/ 4"
 * @param test_case the TestCase to load
 */
static void test_case_11(struct TestCase *test_case)
{
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "/ 4");
    sprintf(test_case->expected_output, "Incomplete expression. Use 'math -h' or 'math -help' for help.\n");
}

#define COLOR_BOLD  "\033[1m"
#define COLOR_OFF   "\033[m"
#define HELP_MSG COLOR_BOLD "\nmath" COLOR_OFF " - command line calculator\n" \
COLOR_BOLD "\nUSAGE\n" COLOR_OFF \
COLOR_BOLD "\tmath " COLOR_OFF "<" COLOR_BOLD "expression" COLOR_OFF ">\n" \
COLOR_BOLD "\nDESCRIPTION\n" COLOR_OFF \
"\tCalculates and displays the result of the mathematical expression <" COLOR_BOLD "expression" COLOR_OFF ">.\n" \
"\tInput operands can be whole numbers or decimal numbers. A single decimal number operand,\n" \
"\tregardless of the number of whole number operands, will make the result a decimal\n" \
"\tnumber.\n" \
"\n\t<" COLOR_BOLD "expression" COLOR_OFF "> may contain spaces. If <" COLOR_BOLD "expression" COLOR_OFF "> contains characters \'/\' or \'*\', it\n" \
"\tmust be wrapped in double quotes (eg: "COLOR_BOLD"\""COLOR_OFF"<"COLOR_BOLD"expression"COLOR_OFF">"COLOR_BOLD"\""COLOR_OFF").\n" \
"\n\tSupported operations are:" \
"\n\t\t" COLOR_BOLD "+" COLOR_OFF " - addition" \
"\n\t\t" COLOR_BOLD "-" COLOR_OFF " - subtraction" \
"\n\t\t" COLOR_BOLD "*" COLOR_OFF " - multiplication" \
"\n\t\t" COLOR_BOLD "/" COLOR_OFF " - division" \
"\n\t\t" COLOR_BOLD "^" COLOR_OFF " - exponentiation\n" \
COLOR_BOLD "\nEXAMPLES\n" COLOR_OFF \
"\tmath 3+4\n\tmath 3 + 4\n\tmath 3*4\n\tmath \"3 * 4\"\n\tmath \"((-20 - 2) * 4.5) / 11)\"\n\n"


/**
 * Test help with "-h"
 * @param test_case the TestCase to load
 */

static void test_case_12(struct TestCase *test_case)
{
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "-h");
    sprintf(test_case->expected_output, HELP_MSG);
}

/**
 * Test help with "-help"
 * @param test_case the TestCase to load
 */
static void test_case_13(struct TestCase *test_case)
{
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "-help");
    sprintf(test_case->expected_output, HELP_MSG);
}

/**
 * Test help with less than 2 args.
 * @param test_case the TestCase to load
 */
static void test_case_14(struct TestCase *test_case)
{
    test_case->input_count = 0;
    test_case->input       = assemble_input(test_case->input_count);
    sprintf(test_case->expected_output, "Argument(s) required. Use 'math -h' or 'math -help' for help.\n");
}

/**
 * Test right-aligned nested parenthesis
 * @param test_case the TestCase to load
 */
static void test_case_15(struct TestCase *test_case)
{
    double ans = (2.5 * (-20 - 2)) + 14;
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count, "(2.5 * (-20 - 2)) + 14");
    sprintf(test_case->expected_output, "%lf\n", ans);
}

/**
 * Test a lot of nested expressions.
 * @param test_case the TestCase to load
 */
static void test_case_16(struct TestCase *test_case)
{
    double ans = ((88 - 87) * 1.25 * (-2 - 2) - (3 + 4 * (69 - -200) * 0.01) * 10);
    test_case->input_count = 1;
    test_case->input       = assemble_input(test_case->input_count,
                                            "((88 - 87) * 1.25 * (-2 - 2) - (3 + 4 * (69 - -200) * 0.01) * 10)");
    sprintf(test_case->expected_output, "%lf\n", ans);
}

#define MATH_PROGRAM_FILENAME "/Users/mud/Projects/cProjects/cmd-line-calculator/math"

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

#endif //CMD_LINE_CALCULATOR_TEST_TESTCASES_H
