#include "diff.h"

#define _ADD(node1, node2) node_ctor(ADD, OP, (node1), (node2))
#define _SUB(node1, node2) node_ctor(SUB, OP, (node1), (node2))
#define _MUL(node1, node2) node_ctor(MUL, OP, (node1), (node2))
#define _DIV(node1, node2) node_ctor(DIV, OP, (node1), (node2))
#define _NUM(val)          node_ctor((val), NUM, NULL, NULL)
#define _VAR(val)          node_ctor((val), VAR, NULL, NULL)
