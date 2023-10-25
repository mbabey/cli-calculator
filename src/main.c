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

Node *expression(Node **curr);

Node *term(Node **curr);

Node *factor(Node **curr);

Node *primary(Node **curr);

/**
 * Get the result of evaluating the expression stored in the abstract syntax tree.
 * @param ast the abstract syntax tree
 * @return pointer to a Token holding the evaluation.
 */
Token *evaluate(Node *ast);

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
    
    // Evaluate the expression
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

#define NUM_BUF_SIZE 16

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
        node->right      = right;
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
        node->right      = right;
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
            (void) fprintf(stderr, "Unmatched parenthesis in expression.\n");
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
    
    if (left->type == dub_t || right->type == dub_t) // Non-terminal.
    {
        if (node->token.type == mult_t)
        {
            left->value.d *= right->value.d;
        } else if (node->token.type == divi_t)
        {
            left->value.d /= right->value.d;
        } else if (node->token.type == add_t)
        {
            left->value.d += right->value.d;
        } else if (node->token.type == sub_t)
        {
            left->value.d -= right->value.d;
        }
    } else // if type is long
    {
        if (node->token.type == mult_t)
        {
            left->value.l *= right->value.l;
        } else if (node->token.type == divi_t)
        {
            left->value.l /= right->value.l;
        } else if (node->token.type == add_t)
        {
            left->value.l += right->value.l;
        } else if (node->token.type == sub_t)
        {
            left->value.l -= right->value.l;
        }
    }
    
    free(right);
    return left;
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
