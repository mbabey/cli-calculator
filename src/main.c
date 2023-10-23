#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

typedef union
{
    long   l;
    double d;
    char   c;
} Value;

typedef enum
{
    long_t,
    dub_t,
    lparen_t,
    rparen_t,
    exp_t,
    mult_t,
    divi_t,
    add_t,
    sub_t,
    ignore_t
} Type;

typedef struct
{
    Value value;
    Type  type;
} Token;

typedef struct list_node
{
    Token            token;
    struct list_node *next;
    struct list_node *prev;
} ListNode;

typedef struct
{
    ListNode *head;
    ListNode *tail;
} List;

List *tokenize(int token_count, char **expression);

void add_node_to_list(List *list, ListNode *ln);

int main(int argc, char **argv)
{
    List *tokens = tokenize(argc - 1, argv + 1);
    
    // Create an AST
    // Evaluate the expression
    
    while (tokens->head != NULL)
    {
        tokens->tail = tokens->head;
        tokens->head = tokens->head->next;
        free(tokens->tail);
    }
    free(tokens);
    
    return 0;
}

#define NUM_BUF_SIZE 16

List *tokenize(int token_count, char **expression)
{
    List *tokens = malloc(sizeof(List));;
    tokens->head = NULL;
    tokens->tail = NULL;
    for (int i = 0; i < token_count; ++i)
    {
        char *curr = expression[i];
        
        for (int j = 0; j < (int) strlen(curr); ++j)
        {
            Token t;
            
            if (isdigit(curr[j]) || curr[j] == '.')
            {
                char buf[NUM_BUF_SIZE] = {'\0'}; // Support up to 15-character numerical values.
                int  buf_i             = 0;
                bool is_dub = false;
                while (buf_i < NUM_BUF_SIZE && (isdigit(curr[j]) || curr[j] == '.'))
                {
                    if (curr[j] == '.') is_dub = true;
                    buf[buf_i++] = curr[j++];
                }
                if (is_dub)
                {
                    t.value.d = strtod(buf, NULL);
                    t.type    = dub_t;
                } else
                {
                    t.value.l = strtol(buf, NULL, 10);
                    t.type    = long_t;
                }
            } else
            {
                t.value.c = curr[j];
                switch (curr[j])
                {
                    case '(':
                        t.type = lparen_t;
                        break;
                    case ')':
                        t.type = rparen_t;
                        break;
                    case '^':
                        t.type = exp_t;
                        break;
                    case '*':
                        t.type = mult_t;
                        break;
                    case '/':
                        t.type = divi_t;
                        break;
                    case '+':
                        t.type = add_t;
                        break;
                    case '-':
                        t.type = sub_t;
                        break;
                    default:
                        t.type = ignore_t;
                }
            }
            
            if (t.type != ignore_t)
            {
                ListNode *ln = malloc(sizeof(ListNode));
                ln->token = t;
                ln->next  = NULL;
                ln->prev  = NULL;
                add_node_to_list(tokens, ln);
            }
        }
    }
    
    return tokens;
}

void add_node_to_list(List *list, ListNode *ln)
{
    if (list->tail)
    {
        ln->prev         = list->tail;
        list->tail->next = ln;
    } else // list is empty.
    {
        list->head = ln;
    }
    list->tail = ln;
}

