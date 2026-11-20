#include "../h_files/diff.h"
#include "../h_files/dsl.h"


error_codes read_equation(FILE *fp, node_t *node, node_t *prev_node)
{
    char user_input[MAX_EQUATION_LEN] = {};
    int file_char = 0;
    error_codes err = SUCCESSFUL_RETURN;

    if ((file_char = getc(fp)) == EOF)
        return SUCCESSFUL_RETURN;
    ungetc(file_char, fp);

    node->parent = prev_node;

    while ((file_char = getc(fp)) != '(');
    fscanf(fp, "%[^()]", user_input);

    file_char = getc(fp);

    if ((err = value_analysis(user_input, node)) != SUCCESSFUL_RETURN)
        return err;

    if (file_char == EOF)
        return SUCCESSFUL_RETURN;

    if (file_char == '(')
    {
        ungetc(file_char, fp);

        if ((err = nodes_checking(node, fp)) != SUCCESSFUL_RETURN)
            return err;

        return SUCCESSFUL_RETURN;
    }

    return SUCCESSFUL_RETURN;
}

bool is_arsign(int sign)
{
    bool is_sign = ((sign == '+') || (sign == '-') || (sign == '*') || (sign == '/'));

    return is_sign;
}

arithmetic_op get_op(int op)
{
    switch (op)
    {
        case '+': return ADD;
        case '-': return SUB;
        case '*': return MUL;
        case '/': return DIV;
        default : return UKS;
    }

    return UKS;
}

bool is_variable(int var)
{
    bool is_var = ((var == 'x') || (var == 'y') || (var == 'z'));

    return is_var;
}

variable get_var(int var)
{
    switch (var)
    {
        case 'x': return VAR_X;
        case 'y': return VAR_Y;
        case 'z': return VAR_Z;
        default   : return UKV;
    }

    return UKV;
}

node_t * node_ctor(double value, type_t type, node_t *left_child, node_t *right_child)
{
    node_t *tmp = (node_t *) calloc(1, sizeof(node_t));
    if (tmp == NULL)
    {
        PRINT_COLOR(EXTRA_RED_, "MEMORY ALLOCATION ERROR WHILE DOING node_ctor\n");
        return NULL;
    }

    switch (type)
    {
        case OP : tmp->value.op     = (arithmetic_op)value; break;
        case NUM: tmp->value.number = value; break;
        case VAR: tmp->value.var    = (variable)value; break;
        default : PRINT_COLOR(EXTRA_RED_, "UNKNOWN VALUE TYPE WHILE DOING node_ctor\n"); return NULL;
    }

    tmp->left_child = left_child;
    tmp->right_child = right_child;
    tmp->parent = NULL;

    return tmp;
}

error_codes tree_dtor(node_t *root)
{
    assert(root);

    if (root == NULL)
        return NULL_ROOT_MEANING;

    if (root->left_child)
        node_dtor(&root->left_child);

    if (root->right_child)
        node_dtor(&root->right_child);

    root->left_child = NULL;
    root->right_child = NULL;
    root->parent = NULL;
    free(root);

    return SUCCESSFUL_RETURN;
}

error_codes node_dtor(node_t **node)
{
    assert(node);

    if ((*node) == NULL)
        return SUCCESSFUL_RETURN;

    node_dtor(&(*node)->left_child);
    node_dtor(&(*node)->right_child);

    (*node)->parent = NULL;

    node_t *tmp = *node;
    *node = NULL;
    free(tmp);

    return SUCCESSFUL_RETURN;
}

error_codes number_analysis(char user_input[], node_t *node)
{
    char *endptr = user_input;
    double number = 0;
    number = strtod(user_input, &endptr);

    if (endptr != user_input)
    {
        if (*endptr != '\0')
        {
            PRINT_COLOR(EXTRA_RED_, "%s не является корректным числовым вводом\n", user_input);
            return WRONG_INPUT;
        }
        else
        {
            node->value.number = number;
            node->type = NUM;
            return SUCCESSFUL_RETURN;
        }
    }

    return NO_NUMBER_FOUND;
}

error_codes operation_analysis(char user_input[], node_t *node)
{
    if (is_arsign(user_input[0]))
    {
        if (user_input[1] == '\0')
        {
            node->value.op = get_op(user_input[0]);
            node->type = OP;

            return SUCCESSFUL_RETURN;
        }
        else
        {
            PRINT_COLOR(EXTRA_RED_, "%s не является корректной арифметической операцией\n", user_input);
            return WRONG_INPUT;
        }
    }

    return NO_OPERATION_FOUND;
}

error_codes variable_analysis(char user_input[], node_t *node)
{
    if (is_variable(user_input[0]))
    {
       if (user_input[1] == '\0')
       {
            node->value.var = get_var(user_input[0]);
            node->type = VAR;

            return SUCCESSFUL_RETURN;
       }
       else
       {
            PRINT_COLOR(EXTRA_RED_, "%s не является корректной переменной\n", user_input);
            return WRONG_INPUT;
       }
    }

    return NO_VARIABLE_FOUND;
}

error_codes value_analysis(char user_input[], node_t *node)
{
    error_codes rtrn = number_analysis(user_input, node);

    if  (rtrn == NO_NUMBER_FOUND)
    {
        if ((rtrn = operation_analysis(user_input, node)) == NO_OPERATION_FOUND)
        {
            if ((rtrn = variable_analysis(user_input, node)) == NO_VARIABLE_FOUND)
            {
                PRINT_COLOR(EXTRA_RED_, "%s не является корректным вводом\n", user_input);
                return WRONG_INPUT;
            }
            return rtrn;
        }
        else
        {
            return rtrn;
        }
    }
    else
        return rtrn;

    return SUCCESSFUL_RETURN;
}

error_codes nodes_checking(node_t *node, FILE *fp)
{
    error_codes err = SUCCESSFUL_RETURN;

    if (node->left_child == NULL)
    {
        node->left_child = _NUM(0);
        if ((err = read_equation(fp, node->left_child, node)) != SUCCESSFUL_RETURN)
            return err;
    }

    if (node->right_child == NULL)
    {
        node->right_child = _NUM(0);
        if ((err = read_equation(fp, node->right_child, node)) != SUCCESSFUL_RETURN)
            return err;
    }

    if (getc(fp) != ')')
        return WRONG_TREE_STRUCTURE;

    return SUCCESSFUL_RETURN;
}

#define RETURN_EVAL_NODE_(sign) {\
    return eval_equation(root->left_child, x_meaning) sign eval_equation(root->right_child, x_meaning);\
}


double eval_equation(node_t *root, double x_meaning)
{
    switch (root->type)
    {
        case NUM: return root->value.number;
        case VAR: return x_meaning;
        case OP :
                 {
                      switch (root->value.op)
                      {
                           case ADD: RETURN_EVAL_NODE_(+);
                           case SUB: RETURN_EVAL_NODE_(-);
                           case MUL: RETURN_EVAL_NODE_(*);
                           case DIV: RETURN_EVAL_NODE_(/);
                           case UKS: return 0;
                      }
                 }
    }

    return 0;
}

#undef RETURN_EVAL_NODE_