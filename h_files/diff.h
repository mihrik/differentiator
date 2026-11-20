#ifndef DIFF_H
#define DIFF_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "colors.h"
#include <assert.h>

const int MAX_EQUATION_LEN = 200;

typedef enum error_codes
{
    SUCCESSFUL_RETURN       = 1,
    WRONG_INPUT             = 10,
    NULL_ROOT_MEANING       = 11,
    MEMORY_ALLOCATION_ERROR = 12,
    WRONG_TREE_STRUCTURE    = 13,
    UNAVAILABLE_FILE        = 14,
    NO_NUMBER_FOUND         = 15,
    NO_OPERATION_FOUND      = 16,
    NO_VARIABLE_FOUND       = 17
} error_codes;

typedef enum arithmetic_op
{
    ADD = 1,
    SUB = 2,
    MUL = 3,
    DIV = 4,
    UKS = 666
} arithmetic_op;

typedef enum variable
{
    VAR_X = 1,
    VAR_Y = 2,
    VAR_Z = 3,
    UKV   = 4
} variable;

typedef enum type_t
{
    NUM = 1,
    VAR = 2,
    OP  = 3
} type_t;

typedef union val_type
{
    double number;
    arithmetic_op op;
    variable var;
} val_type;

typedef struct node_t
{
    type_t type;
    val_type value;
    node_t *left_child;
    node_t *right_child;
    node_t *parent;
} node_t;

bool is_arsign(int sign);
arithmetic_op get_op(int op);
bool is_variable(int var);
variable get_var(int var);
node_t * node_ctor(double value, type_t type, node_t *left_child, node_t *right_child);
error_codes read_equation(FILE *fp, node_t *node, node_t *prev_node);
error_codes node_dtor(node_t **node);
error_codes tree_dtor(node_t *root);
error_codes number_analysis(char user_input[], node_t *node);
error_codes operation_analysis(char user_input[], node_t *node);
error_codes variable_analysis(char user_input[], node_t *node);
error_codes value_analysis(char user_input[], node_t *node);
error_codes nodes_checking(node_t *node, FILE *fp);
double eval_equation(node_t *root, double x_meaning);

#endif