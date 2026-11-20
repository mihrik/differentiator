#include "../h_files/diff.h"
#include "../h_files/dump.h"
#include "../h_files/dsl.h"

int main(void)
{
    FILE *fp = fopen("equation.txt", "r");
    FILE *tex = fopen("latex.tex", "w");

    node_t *root = _NUM(0);

    read_equation(fp, root, NULL);

    print_html(root, FNAME, PIC_NAME);
    latex_dump(tex, root);

    printf("if x is equal to 0 - equation is equal to: %lg\n", eval_equation(root, 0));

    tree_dtor(root);


    fclose(fp);
    fclose(tex);

    return 0;
}