#include "../h_files/diff.h"
#include "../h_files/dump.h"
#include "../h_files/dsl.h"

int main(void)
{
    FILE *fp = fopen("equation.txt", "r");
    FILE *tex = fopen("latex.tex", "w");

    node_t *root = _NUM(0);
    bool error = false;

    read_equation(fp, root, NULL);
    node_t *simplified_root = copy_node(root, NULL);
    simplifier(&simplified_root, &error);
    latex_dump(tex, simplified_root);
    if (error)
    {
        tree_dtor(root);
        fclose(fp);
        fclose(tex);
        tree_dtor(simplified_root);

        return 0;
    }


    print_html(simplified_root, FNAME, PIC_NAME);

    node_t *first_div = find_derivative(simplified_root, &error);
    if (error)
    {
        tree_dtor(first_div);
        tree_dtor(root);
        fclose(fp);
        fclose(tex);
        tree_dtor(simplified_root);

        return 0;
    }
    //printf("%lg = f'(0)\n", eval_equation(first_div, 0));
    latex_dump(tex, first_div);
    print_html(first_div, FNAME, PIC_NAME);

    tree_dtor(root);
    tree_dtor(first_div);
    tree_dtor(simplified_root);

    fclose(fp);
    fclose(tex);

    return 0;
}