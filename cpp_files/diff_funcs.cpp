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
    bool is_sign = ((sign == '+') || (sign == '-') || (sign == '*') || (sign == '/') || (sign == '^'));

    return is_sign;
}

bool is_complex_op(char user_input[])
{
    return (strcmp(user_input, "log") == 0) ? true : false;
}

arithmetic_op get_op(int op)
{
    switch (op)
    {
        case '+': return ADD;
        case '-': return SUB;
        case '*': return MUL;
        case '/': return DIV;
        case '^': return EXP;
        default : return UKS;
    }

    return UKS;
}

arithmetic_op get_complex_op(char user_input[])
{
    if (strcmp(user_input, "log") == 0)
        return LOG;

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

    tmp->type = type;
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
    if (is_arsign(user_input[0]) || is_complex_op(user_input))
    {
        if (user_input[1] == '\0')
        {
            node->value.op = get_op(user_input[0]);
            node->type = OP;

            return SUCCESSFUL_RETURN;
        }
        else if (is_complex_op(user_input))
        {
            node->value.op = get_complex_op(user_input);
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
                           case EXP: return pow(eval_equation(root->left_child, x_meaning), eval_equation(root->right_child, x_meaning));
                           case LOG: return log(eval_equation(root->right_child, x_meaning)) / log(eval_equation(root->left_child, x_meaning));
                           case UKS: return 0;
                      }
                 }
    }

    return 0;
}

#undef RETURN_EVAL_NODE_


node_t *copy_node(const node_t *node, node_t *parent)
{
    if (node == NULL)
        return NULL;

    node_t *tmp = (node_t *) calloc(1, sizeof(node_t));
    if (tmp == NULL)
    {
        PRINT_COLOR(EXTRA_RED_, "MEMORY ALLOCATION ERROR WHILE DOING copy_ctor\n");
        return NULL;
    }

    tmp->parent = parent;
    tmp->value = node->value;
    tmp->type = node->type;

    tmp->left_child = copy_node(node->left_child, tmp);
    tmp->right_child = copy_node(node->right_child, tmp);

    return tmp;
}

node_t * diff(const node_t *node)
{
    switch (node->type)
    {
        case NUM: return _NUM(0);
        case VAR: return _NUM(node->value.var == VAR_X);
        case OP :
                 {
                      switch (node->value.op)
                      {
                           case ADD: return _ADD(_DL, _DR);
                           case SUB: return _SUB(_DL, _DR);
                           case MUL: return _ADD(_MUL(_DL, _CR), _MUL(_CL, _DR));
                           case DIV: return _DIV(_SUB(_MUL(_DL, _CR), _MUL(_CL, _DR)), _MUL(_CR, _CR));
                           case LOG: return _DIV(_SUB(_DIV(_MUL(_DR, _LN(_CL)), _CR), _DIV(_MUL(_DL, _LN(_CR)), _CL)), _MUL(_LN(_CL), _LN(_CL)));
                           case EXP: return _MUL(_EXP(_CL, _CR), _ADD(_DIV(_MUL(_CR, _DL), _CL), _MUL(_LN(_CL), _DR)));
                           case UKS: return NULL;
                      }
                 }
    }

    return NULL;
}

void set_parent(node_t *root)
{
    if (root->left_child != NULL)
    {
        root->left_child->parent = root;
        set_parent(root->left_child);
    }

    if (root->right_child != NULL)
    {
        root->right_child->parent = root;
        set_parent(root->right_child);
    }
}

node_t * find_derivative(node_t *node, bool *error)
{
    node_t *derivative = diff(node);
    simplifier(&node, error);

    return derivative;
}

#define SUBSTITUTE_TO_ONE_ {\
    tree_dtor(*node);\
    *node = _NUM(1);\
    (*changes)++;\
    return;\
}

#define SUBSTITUTE_TO_ZERO_ {\
    tree_dtor(*node);\
    *node = _NUM(0);\
    (*changes)++;\
    return;\
}

#define SUBSTITUTE_TO_LEFT_ {\
    node_t *tmp2 = copy_node((*node)->left_child, (*node)->parent);\
    tree_dtor(*node);\
    *node = tmp2;\
    (*changes)++;\
    return;\
}

#define SUBSTITUTE_TO_RIGHT_ {\
    node_t *tmp2 = copy_node((*node)->right_child, (*node)->parent);\
    tree_dtor(*node);\
    *node = tmp2;\
    (*changes)++;\
    return;\
}

bool is_num(node_t *node)
{
    return node->type == NUM;
}

bool is_op(node_t *node)
{
    return node->type == OP;
}

bool node_is_number(node_t *node, double num)
{
    return node->type == NUM && is_zero(node->value.number - num);
}

bool equal_vars(node_t *node1, node_t *node2)
{
    return node1->type == VAR && node2->type == VAR && node2->value.var == node1->value.var;
}

void simplify_add(node_t **node, int *changes)
{
    if (node_is_number((*node)->left_child, 0))
    {
        SUBSTITUTE_TO_RIGHT_;
    }
    if (node_is_number((*node)->right_child, 0))
    {
        SUBSTITUTE_TO_LEFT_;
    }
}

void simplify_sub(node_t **node, int *changes)
{
    if (node_is_number((*node)->left_child, 0) && is_num((*node)->right_child))
    {
        (*node)->right_child->value.number = -(*node)->right_child->value.number;
        SUBSTITUTE_TO_RIGHT_;
    }
    if (node_is_number((*node)->right_child, 0))
    {
        SUBSTITUTE_TO_LEFT_;
    }
}

void simplify_mul(node_t **node, int *changes)
{
    if (node_is_number((*node)->left_child, 0))
    {
        SUBSTITUTE_TO_ZERO_;
    }
    if (node_is_number((*node)->right_child, 0))
    {
        SUBSTITUTE_TO_ZERO_;
    }
    if (node_is_number((*node)->right_child, 1))
    {
        SUBSTITUTE_TO_LEFT_;
    }
    if (node_is_number((*node)->left_child, 1))
    {
        SUBSTITUTE_TO_RIGHT_;
    }
}


void simplify_div(node_t **node, int* changes)
{
    if (node_is_number((*node)->left_child, 0))
    {
            SUBSTITUTE_TO_ZERO_;
    }
    if (equal_vars((*node)->left_child, (*node)->right_child))
    {
        SUBSTITUTE_TO_ONE_;
    }
}

void check_div(node_t **node, bool *error)
{
    if (node_is_number((*node)->right_child, 0))
    {
        PRINT_COLOR(EXTRA_RED_, "Деление на ноль\n");
        *error = true;
        return;
    }
}

void simplify_exp(node_t **node, int *changes)
{
    if (node_is_number((*node)->right_child, 0))
    {
        SUBSTITUTE_TO_ONE_;
    }
    if (node_is_number((*node)->right_child, 1))
    {
        SUBSTITUTE_TO_LEFT_;
    }
    if (node_is_number((*node)->left_child, 1))
    {
        SUBSTITUTE_TO_ONE_;
    }
    if (node_is_number((*node)->left_child, 0) && is_num((*node)->right_child))
    {
        SUBSTITUTE_TO_ZERO_;
    }
}

void check_exp(node_t **node, bool *error)
{
    if (node_is_number((*node)->left_child, 0) && is_num((*node)->right_child))
    {
        if ((*node)->right_child->value.number < -EPSILON)
        {
            PRINT_COLOR(EXTRA_RED_, "Ошибка деления на ноль вида [0^(-x), x > 0]\n");
            *error = true;
            return;
        }
    }
    if (node_is_number((*node)->right_child, 0))
    {
        if (node_is_number((*node)->left_child, 0))
        {
            PRINT_COLOR(EXTRA_RED_, "Неопределённость вида [0^0]\n");
            *error = true;
            return;
        }
    }
}

void simplify_log(node_t **node, int *changes)
{
    if (node_is_number((*node)->right_child, 1) && is_num((*node)->left_child))
    {
        SUBSTITUTE_TO_ZERO_;
    }
}

void check_log(node_t **node, bool *error)
{
    if ((*node)->left_child->type == NUM && (is_zero((*node)->left_child->value.number - 1) || (*node)->left_child->value.number < -EPSILON || is_zero((*node)->left_child->value.number)))
    {
            PRINT_COLOR(EXTRA_RED_, "Основание логарифма должно быть положительным и отличаться от единицы\n");
            *error = true;
            return;
    }
    if ((*node)->right_child->type == NUM && ((*node)->right_child->value.number < -EPSILON || is_zero((*node)->right_child->value.number)))
    {
            PRINT_COLOR(EXTRA_RED_, "Подлогарифмическое выражение должно быть положительным\n");
            *error = true;
            return;
    }
}

#undef SUBSTITUTE_TO_ONE_
#undef SUBSTITUTE_TO_ZERO_
#undef SUBSTITUTE_TO_LEFT_
#undef SUBSTITUTE_TO_RIGHT_

void simplify_operation(node_t **node, int *changes, bool *error)
{
    switch((*node)->value.op)
    {
        case ADD:
        {
            simplify_add(node, changes); break;
        }
        case MUL:
        {
            simplify_mul(node, changes); break;
        }
        case DIV:
        {
            check_div(node, error);
            if (*error)
                return;

            simplify_div(node, changes); break;
        }
        case EXP:
        {
            check_exp(node, error);
            if (*error)
                return;

            simplify_exp(node, changes); break;
        }
        case SUB:
        {
            simplify_sub(node, changes); break;
        }
        case LOG:
        {
            check_log(node, error);
            if (*error)
                return;

            simplify_log(node, changes); break;
        }
        case UKS: *error = true;
    }
}

void delete_neutrals(node_t **node, int *changes, bool *error)
{
    if (*error == true)
        return;

    if ((*node)->right_child == NULL && (*node)->left_child == NULL)
        return;

    delete_neutrals(&(*node)->left_child, changes, error);
    delete_neutrals(&(*node)->right_child, changes, error);

    if ((*node)->type == OP)
    {
        simplify_operation(node, changes, error);
    }
}

#define SUBSTITUTE_TO_(expr) {\
    node_t *tmp = _NUM(expr);\
    tree_dtor(*node);\
    *node = tmp;\
    (*changes)++;\
    return;\
}

bool is_zero(double num)
{
    return fabs(num) < EPSILON;
}

void delete_similar(node_t **node, int *changes, bool *error)
{
    if (*error)
        return;

    if ((*node)->right_child == NULL && (*node)->left_child == NULL)
        return;

    delete_similar(&(*node)->left_child, changes, error);
    delete_similar(&(*node)->right_child, changes, error);

    bool may_be_simplified = (is_op((*node)) && is_num((*node)->left_child) && is_num((*node)->right_child));

    if (may_be_simplified)
    {
        similar_ops(node, changes, error);
    }
}

void similar_ops(node_t **node, int *changes, bool *error)
{
    double num1 = (*node)->left_child->value.number;
    double num2 = (*node)->right_child->value.number;

    switch ((*node)->value.op)
    {
        case ADD:
        {
            SUBSTITUTE_TO_(num1 + num2);
        }
        case SUB:
        {
            SUBSTITUTE_TO_(num1 - num2);
        }
        case MUL:
        {
            SUBSTITUTE_TO_(num1 * num2);
        }
        case DIV:
        {
            check_div(node, error);
            if (*error)
                return;

            SUBSTITUTE_TO_(num1 / num2);
        }
        case EXP:
        {
            check_exp(node, error);
            if (*error)
                return;

            SUBSTITUTE_TO_(pow(num1, num2));
        }
        case LOG:
        {
            check_log(node, error);
            if (*error)
                return;

            SUBSTITUTE_TO_(log(num2) / log(num1));
        }
        case UKS: *error = true;
    }
}

#undef SUBSTITUTE_TO_

void simplifier(node_t **root, bool *error)
{
    int changes = 0;

    do
    {
        changes = 0;
        delete_neutrals(root, &changes, error);
        delete_similar(root, &changes, error);
        set_parent(*root);
    } while (changes != 0);
}