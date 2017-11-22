#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "list.h"
#include "utils.h"
#include "item.h"
#include "shelf.h"
#include <string.h>
#include "common.h"

struct vara;

struct regret;

item_t *new_element(char *namn, char *desc, int price, list_t *shelflist);

void print_menu();

void print_tree_ltr_prim(struct node *n, int *counter, int *page);

int print_tree_ltr(tree_t *t);

void free_elements(elem_t name, elem_t elem);

struct regret *new_regret(int code, struct node *nod, elem_t *vara, struct list *lista);

void print_edit(item_t *temp);

void print_vara(elem_t vara);

struct list *copy_list(struct list *l);

void regret_action(struct regret *regret, struct tree *db);

int event_loop(tree_t *db);

int list_contains_shelf(struct list *l, char *shelf);
