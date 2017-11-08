#include "tree.h"
#include "list.h"

struct item;

struct LM;

typedef struct item item_t;

//Prints the menu for adding and item to the database
//
//
//
int new_item_menu();

struct regret *new_item(tree_t *db);

void *delete_item(tree_t *db);

item_t *create_element(char *namn, char *desc, int price, list_t *shelflist);

void free_shelflist(elem_t *elem);

struct regret *edit_item(tree_t *db);
