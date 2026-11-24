#include "../h_files/dump.h"
#include "../h_files/colors.h"

static int pic_num = 0;
static int node_num = 0;

error_codes print_spears(node_t *root, FILE *png)
{
    node_num++;

    if (node_num != 0)
    {
        fprintf(png, "node%d [color=\"BLACK\", penwidth=1];\n", node_num);
    }

    int cur_node = node_num;

    dump_val(png, root);

    if (root->left_child)
    {
        fprintf(png, "node%d -> ", cur_node);
        print_spears(root->left_child, png);
    }
    if (!is_unary(root))
    {
        if (root->right_child)
        {
            fprintf(png, "node%d -> ", cur_node);
            print_spears(root->right_child, png);
        }
    }

    return SUCCESSFUL_RETURN;
}

error_codes prepare_png(node_t *root, const char fname[])
{

    FILE *png = fopen(fname, "w+");

    if (png == NULL)
    {
        PRINT_COLOR(EXTRA_RED_, "UNAVAILABLE FILE: <%s>, while doing prepare_png\n", fname);
        return UNAVAILABLE_FILE;
    }

    fprintf(png, "digraph {\n\n");
    fprintf(png, "splines=line;\n");

    node_num = 0;
    print_spears(root, png);

    fprintf(png, "}\n");

    fclose(png);

    return SUCCESSFUL_RETURN;
}

error_codes print_html(node_t *root, const char fname[], const char pic_name[])
{
    FILE *png = NULL;
    if (pic_num == 0)
        png = fopen(fname, "w");
    else
        png = fopen(fname, "a");

    if (png == NULL)
    {
        PRINT_COLOR(EXTRA_RED_, "UNAVAILABLE FILE: <%s>, while doing prepare_png\n", fname);
        return UNAVAILABLE_FILE;
    }

    pic_num++;

    char pic_file_name[MAX_FILE_NAME_LEN] = {};
    sprintf(pic_file_name, "txt_files/%s%d.txt", pic_name, pic_num);

    char png_file_name[MAX_FILE_NAME_LEN] = {};
    sprintf(png_file_name, "png_files/%s%d.png", pic_name, pic_num);


    prepare_png(root, pic_file_name);
    char system_command[MAX_COMMAND_NAME_LEN] = {};
    sprintf(system_command, "dot %s -Tpng -o %s", pic_file_name, png_file_name);
    system(system_command);

    fprintf(png, "<img src=%s width=1200px>\n", png_file_name);

    fclose(png);

    return SUCCESSFUL_RETURN;
}

void dump_num(FILE *png, node_t *root)
{
    fprintf(png, "node%d [shape=box; style = filled; fillcolor = \"#6224c7\"; color = \"#000000\";\n", node_num);
    fprintf(png, "  label = \"%lg\"; ]\n\n", root->value.number);
}

#define PRINT_VAR_NODE_(x) {\
    fprintf(png, "node%d [shape=triangle; style = filled; fillcolor = \"#27d921\"; color = \"#000000\";\n", node_num);\
    fprintf(png, "  label = \"%s\"; ]\n\n", #x);\
}

void dump_var(FILE *png, node_t *root)
{
    switch (root->value.var)
    {
        case VAR_X:
                    {
                        PRINT_VAR_NODE_(X);

                        break;
                    }
        case VAR_Y:
                    {
                        PRINT_VAR_NODE_(Y);

                        break;
                    }
        case VAR_Z:
                    {
                        PRINT_VAR_NODE_(Z);

                        break;
                    }
        case UKV  :
                    {
                        break;
                    }
    }
}

#undef PRINT_VAR_NODE

#define PRINT_NODE_(x) {\
    fprintf(png, "node%d [shape=circle; style = filled; fillcolor = \"#bdc951\"; color = \"#000000\";\n", node_num);\
    fprintf(png, "  label = \"%s\"; ]\n\n", #x);\
}

void dump_op(FILE *png, node_t *root)
{
    switch (root->value.op)
    {
        case ADD   :
        {
            PRINT_NODE_(+);

            break;
        }
        case MUL   :
        {
            PRINT_NODE_(*);

            break;
        }
        case SUB   :
        {
            PRINT_NODE_(-);

            break;
        }
        case DIV   :
        {
            PRINT_NODE_(/);

            break;
        }
        case EXP   :
        {
            PRINT_NODE_(^);

            break;
        }
        case LOG   :
        {
            PRINT_NODE_(LOG);

            break;
        }
        case COS   :
        {
            PRINT_NODE_(COS);

            break;
        }
        case SIN   :
        {
            PRINT_NODE_(SIN);

            break;
        }
        case TAN   :
        {
            PRINT_NODE_(TAN);

            break;
        }
        case COT   :
        {
            PRINT_NODE_(COT);

            break;
        }
        case ARCCOS:
        {
            PRINT_NODE_(ARCCOS);

            break;
        }
        case ARCSIN:
        {
            PRINT_NODE_(ARCSIN);

            break;
        }
        case ARCTAN:
        {
            PRINT_NODE_(ARCTAN);

            break;
        }
        case ARCCOT:
        {
            PRINT_NODE_(ARCCOT);

            break;
        }
        case UKS:
        {
            PRINT_NODE_(Unknown_sign);

            break;
        }
    }
}

#undef PRINT_NODE_

void dump_val(FILE *png, node_t *root)
{
    switch (root->type)
    {
        case NUM:
                 {
                    dump_num(png, root);

                    break;
                 }
        case VAR:
                 {
                    dump_var(png, root);

                    break;
                 }
        case OP :
                 {
                    dump_op(png, root);

                    break;
                 }
    }
}

void latex_dump(FILE *tex, node_t *root)
{
    fprintf(tex, "$");

    tex_dump_type(tex, root);

    fprintf(tex, "$\n");
}

void tex_dump_type(FILE *tex, node_t *root)
{
    switch (root->type)
    {
        case NUM:
        {
            if (root->value.number < -EPSILON)
            {
                fprintf(tex, "(%lg)", root->value.number);

                break;
            }
            fprintf(tex, "{%lg}", root->value.number);

            break;
        }
        case VAR:
        {
            var_latex_dump(tex, root);

            break;
        }
        case OP:
        {
            op_latex_dump(tex, root);
        }
    }
}

void sub_latex_dump(FILE *tex, node_t *root)
{
    fprintf(tex, "(");
    tex_dump_type(tex, root->left_child);
    fprintf(tex, "-");
    tex_dump_type(tex, root->right_child);
    fprintf(tex, ")");
}

void add_latex_dump(FILE *tex, node_t *root)
{
    fprintf(tex, "(");
    tex_dump_type(tex, root->left_child);
    fprintf(tex, "+");
    tex_dump_type(tex, root->right_child);
    fprintf(tex, ")");
}

void mul_latex_dump(FILE *tex, node_t *root)
{
    fprintf(tex, "{");
    tex_dump_type(tex, root->left_child);
    fprintf(tex, "\\cdot");
    tex_dump_type(tex, root->right_child);
    fprintf(tex, "}");
}

void div_latex_dump(FILE *tex, node_t *root)
{
    fprintf(tex, "\\frac");
    fprintf(tex, "{");
    tex_dump_type(tex, root->left_child);
    fprintf(tex, "}{");
    tex_dump_type(tex, root->right_child);
    fprintf(tex, "}");
}

void exp_latex_dump(FILE *tex, node_t *root)
{
    fprintf(tex, "{");
    tex_dump_type(tex, root->left_child);
    fprintf(tex, "}");
    fprintf(tex, "^");
    fprintf(tex, "{");
    tex_dump_type(tex, root->right_child);
    fprintf(tex, "}");
}

void cos_latex_dump(FILE *tex, node_t *root)
{
    fprintf(tex, "\\cos(");
    tex_dump_type(tex, root->left_child);
    fprintf(tex, ")");
}

void sin_latex_dump(FILE *tex, node_t *root)
{
    fprintf(tex, "\\sin(");
    tex_dump_type(tex, root->left_child);
    fprintf(tex, ")");
}

void tan_latex_dump(FILE *tex, node_t *root)
{
    fprintf(tex, "\\tan(");
    tex_dump_type(tex, root->left_child);
    fprintf(tex, ")");
}

void cot_latex_dump(FILE *tex, node_t *root)
{
    fprintf(tex, "\\cot(");
    tex_dump_type(tex, root->left_child);
    fprintf(tex, ")");
}

void arccos_latex_dump(FILE *tex, node_t *root)
{
    fprintf(tex, "\\cos^{-1}(");
    tex_dump_type(tex, root->left_child);
    fprintf(tex, ")");
}

void arcsin_latex_dump(FILE *tex, node_t *root)
{
    fprintf(tex, "\\sin^{-1}(");
    tex_dump_type(tex, root->left_child);
    fprintf(tex, ")");
}

void arctan_latex_dump(FILE *tex, node_t *root)
{
    fprintf(tex, "\\tan^{-1}(");
    tex_dump_type(tex, root->left_child);
    fprintf(tex, ")");
}

void arccot_latex_dump(FILE *tex, node_t *root)
{
    fprintf(tex, "\\cot^{-1}(");
    tex_dump_type(tex, root->left_child);
    fprintf(tex, ")");
}

void log_latex_dump(FILE *tex, node_t *root)
{
    if (root->left_child->type == NUM && is_zero(root->left_child->value.number - exp(1)))
    {
        fprintf(tex, "\\ln(");
        tex_dump_type(tex, root->right_child);
        fprintf(tex, ")");
    }
    else
    {
        fprintf(tex, "\\log");
        fprintf(tex, "_{");
        tex_dump_type(tex, root->left_child);
        fprintf(tex, "}(");
        tex_dump_type(tex, root->right_child);
        fprintf(tex, ")");
    }
}

void op_latex_dump(FILE *tex, node_t *root)
{
    switch (root->value.op)
    {
        case ADD   :
        {
            add_latex_dump(tex, root);

            break;
        }
        case SUB   :
        {
            sub_latex_dump(tex, root);

            break;
        }
        case MUL   :
        {
            mul_latex_dump(tex, root);

            break;
        }
        case DIV   :
        {
            div_latex_dump(tex, root);

            break;
        }
        case EXP   :
        {
            exp_latex_dump(tex, root);

            break;
        }
        case LOG   :
        {
            log_latex_dump(tex, root);

            break;
        }
        case COS   :
        {
            cos_latex_dump(tex, root);

            break;
        }
        case SIN   :
        {
            sin_latex_dump(tex, root);

            break;
        }
        case TAN   :
        {
            tan_latex_dump(tex, root);

            break;
        }
        case COT   :
        {
            cot_latex_dump(tex, root);

            break;
        }
        case ARCCOS:
        {
            arccos_latex_dump(tex, root);

            break;
        }
        case ARCSIN:
        {
            arcsin_latex_dump(tex, root);

            break;
        }
        case ARCTAN:
        {
            arctan_latex_dump(tex, root);

            break;
        }
        case ARCCOT:
        {
            arccot_latex_dump(tex, root);

            break;
        }
        case UKS   : break;
    }
}

void var_latex_dump(FILE *tex, node_t *root)
{
    switch (root->value.var)
    {
        case VAR_X: fprintf(tex, "{%c}", 'x'); break;
        case VAR_Y: fprintf(tex, "{%c}", 'y'); break;
        case VAR_Z: fprintf(tex, "{%c}", 'z'); break;
        case UKV  : break;
    }
}