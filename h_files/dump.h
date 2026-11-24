#ifndef DUMP_H
#define DUMP_H
#include "diff.h"

const char * const PIC_NAME = "MEOW";
const char * const FNAME = "LOGFILE.html";
const int MAX_FILE_NAME_LEN = 40;
const int MAX_COMMAND_NAME_LEN = 120;
const int MAX_OUTPUT_LEN = 500;

error_codes prepare_png(node_t *root, const char fname[]);
error_codes print_html(node_t *root, const char fname[], const char pic_name[]);
error_codes print_spears(node_t *root, FILE *png);
void dump_num(FILE *png, node_t *root);
void dump_var(FILE *png, node_t *root);
void dump_op(FILE *png, node_t *root);
void dump_val(FILE *png, node_t *root);
void tex_dump_type(FILE *tex, node_t *root);
void latex_dump(FILE *tex, node_t *root);
void sub_latex_dump(FILE *tex, node_t *root);
void add_latex_dump(FILE *tex, node_t *root);
void mul_latex_dump(FILE *tex, node_t *root);
void div_latex_dump(FILE *tex, node_t *root);
void op_latex_dump(FILE *tex, node_t *root);
void var_latex_dump(FILE *tex, node_t *root);
void exp_latex_dump(FILE *tex, node_t *root);
void log_latex_dump(FILE *tex, node_t *root);

#endif