#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tree.h"
#include "list.h"
#include "utils.h"
#include <string.h>


typedef struct shelf shelf_t;

struct shelf;

struct LM;


void does_node_have_shelf(node_t *n, char *shelf, bool *exists);

void is_shelf_taken_prim(struct node* n, char *shelf, bool *exists);

bool is_shelf_taken(struct tree *db, char *shelf);
