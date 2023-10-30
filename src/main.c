#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

/* TODO:
 *  - Documentation
 *  - Add support for exponents
 *  - Add support for negative numbers
 *  - Add support for nested parentheses
 *  - Handle errors: tokenizing, parsing, evaluating
 *  - Add -help or -h flag to offer instructions of use.
 */

typedef union
{
    long   l;
    double d;
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

typedef struct node
{
    Token       token;
    struct node *right;
    struct node *left;
} Node;

typedef struct
{
    Node *head;
    Node *tail;
} List;

/**
 * Tokenize an input string expression.
 * @param arg_count the number of expression-related command line arguments.
 * @param expression the input string expression
 * @return a doubly linked list of tokens
 */
List *tokenize(int arg_count, char **expression);

/**
 * Add a Node to the tail of a doubly linked list
 * @param list the list to which the node will be added
 * @param node the node to add
 */
void add_node_to_list(List *list, Node *node);

/**
 * Parse tokens and create an abstract syntax tree based on the following grammar:
 * expression   -> term
 * term         -> factor ( ("+" | "-") factor)*
 * factor       -> primary ( ("*" | "/") primary)*
 * primary      -> NUMBER | "(" expression ")"
 * @param tokens the tokens to parse
 * @return an abstract syntax tree representation of the tokens
 */
Node *parse(List *tokens);

/**
 * Parse an expression from the input.
 * @param curr the current token
 * @return the root Node of the expression
 */
Node *expression(Node **curr);

/**
 * Parse a term from an expression.
 * @param curr the current token
 * @return the root Node of the term
 */
Node *term(Node **curr);

/**
 * Parse a factor from an term.
 * @param curr the current token
 * @return the root Node of the factor
 */
Node *factor(Node **curr);

/**
 * Parse a primary from a factor.
 * @param curr the current token
 * @return the primary as a Node, or the root Node of a parenthesized expression
 */
Node *primary(Node **curr);

/**
 * Get the result of evaluating the expression stored in the abstract syntax tree.
 * @param ast the abstract syntax tree
 * @return pointer to a Token holding the evaluation.
 */
Token *evaluate(Node *ast);

/**
 * Perform a mathematical operation based on the parameter Tokens and store the result in left.
 * The operation (multiply, divide, add, subtract) is stored in operation. The left and right
 * operands are stored in left and right. If either left or right is a double, the result
 * will be returned as a double.
 * @param operation Token holding the operation to perform
 * @param left Token holding the left operand
 * @param right Token holding the right operand
 * @return a pointer to the Token holding the result
 */
void do_math(Token *operation, Token *left, Token *right);

/**
 * Free a doubly linked list.
 * @param list the list to free.
 */
void free_list(List *list);

/**
 * Free an abstract syntax tree.
 * @param ast_root the root node of the tree to free.
 */
void free_ast(Node *ast_root);

int main(int argc, char **argv)
{
    List *tokens = tokenize(argc - 1, argv + 1);
    
    Node *ast = parse(tokens);
    
    free_list(tokens);
    
    Token *ans = evaluate(ast);
    
    if (ans->type == dub_t)
    {
        printf("%lf\n", ans->value.d);
    } else
    {
        printf("%ld\n", ans->value.l);
    }
    
    free_ast(ast);
    free(ans);
    
    return 0;
}

#define IS_NUMERIC(num_str) \
    isdigit((num_str)[j]) || (num_str)[j] == '.' || ((num_str)[j] == '-' && isdigit((num_str)[j + 1]))
#define NUM_BUF_SIZE 16 // Support up to 15-digit numerical values.

List *tokenize(int arg_count, char **expression)
{
    List *tokens = malloc(sizeof(List));;
    tokens->head = NULL;
    tokens->tail = NULL;
    for (int i = 0; i < arg_count; ++i)
    {
        char *curr = expression[i];
        
        for (int j = 0; j < (int) strlen(curr);)
        {
            Token t;
            
            if (IS_NUMERIC(curr))
            {
                char buf[NUM_BUF_SIZE] = {'\0'};
                int  buf_i             = 0;
                bool is_dub = false;
                while (buf_i < NUM_BUF_SIZE && (IS_NUMERIC(curr)))
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
                t.value.l = 0;
                switch (curr[j++])
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
                Node *ln = malloc(sizeof(Node));
                ln->token = t;
                ln->right = NULL;
                ln->left  = NULL;
                add_node_to_list(tokens, ln);
            }
        }
    }
    
    return tokens;
}

void add_node_to_list(List *list, Node *node)
{
    if (list->tail)
    {
        node->left        = list->tail;
        list->tail->right = node;
    } else // list is empty.
    {
        list->head = node;
    }
    list->tail = node;
}

Node *parse(List *tokens)
{
    Node *curr = tokens->head;
    return expression(&curr);
}

Node *expression(Node **curr) // Current MUST be updated.
{
    return term(curr);
}

Node *term(Node **curr)
{
    Node *node;
    
    node = factor(curr);
    
    while ((*curr)->right && ((*curr)->right->token.type == add_t || (*curr)->right->token.type == sub_t))
    {
        Node *left = node;
        node = malloc(sizeof(Node));
        node->token.type = (*curr)->right->token.type;
        node->left       = left;
        *curr = (*curr)->right->right;
        Node *right = factor(curr);
        node->right = right;
    }
    
    return node;
}

Node *factor(Node **curr)
{
    Node *node;
    
    node = primary(curr); // Will be NULL if problem.
    
    while ((*curr)->right && ((*curr)->right->token.type == mult_t || (*curr)->right->token.type == divi_t))
    {
        Node *left = node;
        node = malloc(sizeof(Node));
        node->token.type = (*curr)->right->token.type;
        node->left       = left;
        *curr = (*curr)->right->right;
        Node *right = primary(curr);
        node->right = right;
    }
    
    return node;
}

Node *primary(Node **curr)
{
    Node *node;
    
    if ((*curr)->token.type == dub_t || (*curr)->token.type == long_t)
    {
        node = malloc(sizeof(Node));
        node->token.type = (*curr)->token.type;
        if (node->token.type == dub_t)
        {
            node->token.value.d = (*curr)->token.value.d;
        } else
        {
            node->token.value.l = (*curr)->token.value.l;
        }
        node->left       = NULL;
        node->right      = NULL;
    } else if ((*curr)->token.type == lparen_t)
    {
        *curr = (*curr)->right;
        node = expression(curr);
        Node *rparen = (*curr);
        while (rparen && rparen->token.type != rparen_t)
        {
            rparen = rparen->right;
        }
        if (!rparen)
        {
            (void) fprintf(stdout, "Unmatched parenthesis in expression.\n");
            return NULL;
        }
        *curr = rparen;
    } else
    {
        node = NULL;
    }
    
    return node;
}

Token *evaluate(Node *node)
{
    Token *left, *right;
    if (node->left)
    {
        left = evaluate(node->left);
    }
    if (node->right)
    {
        right = evaluate(node->right);
    }
    
    if (node->token.type == long_t || node->token.type == dub_t) // Terminal value.
    {
        Token *ret = malloc(sizeof(Token));
        memcpy(ret, &node->token, sizeof(Token));
        return ret;
    }
    
    do_math(&node->token, left, right);
    free(right);
    
    return left;
}

void do_math(Token *operation, Token *left, Token *right)
{
    if (left->type == dub_t && right->type == long_t)
    {
        if (operation->type == mult_t)
        {
            left->value.d *= (double) right->value.l;
        } else if (operation->type == divi_t)
        {
            left->value.d /= (double) right->value.l;
        } else if (operation->type == add_t)
        {
            left->value.d += (double) right->value.l;
        } else if (operation->type == sub_t)
        {
            left->value.d -= (double) right->value.l;
        }
    } else if (left->type == long_t && right->type == dub_t)
    {
        left->type = dub_t;
        if (operation->type == mult_t)
        {
            left->value.d = (double) left->value.l * right->value.d;
        } else if (operation->type == divi_t)
        {
            left->value.d = (double) left->value.l / right->value.d;
        } else if (operation->type == add_t)
        {
            left->value.d = (double) left->value.l + right->value.d;
        } else if (operation->type == sub_t)
        {
            left->value.d = (double) left->value.l - right->value.d;
        }
    } else if (left->type == dub_t && right->type == dub_t)
    {
        if (operation->type == mult_t)
        {
            left->value.d *= right->value.d;
        } else if (operation->type == divi_t)
        {
            left->value.d /= right->value.d;
        } else if (operation->type == add_t)
        {
            left->value.d += right->value.d;
        } else if (operation->type == sub_t)
        {
            left->value.d -= right->value.d;
        }
    } else // left == long && right == long
    {
        if (operation->type == mult_t)
        {
            left->value.l *= right->value.l;
        } else if (operation->type == divi_t)
        {
            left->value.l /= right->value.l;
        } else if (operation->type == add_t)
        {
            left->value.l += right->value.l;
        } else if (operation->type == sub_t)
        {
            left->value.l -= right->value.l;
        }
    }
}

void free_list(List *list)
{
    while (list->head != NULL)
    {
        list->tail = list->head;
        list->head = list->head->right;
        free(list->tail);
    }
    free(list);
}

void free_ast(Node *ast)
{
    if (ast->left) free_ast(ast->left);
    if (ast->right) free_ast(ast->right);
    free(ast);
}
