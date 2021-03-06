#include "tree.h"
#include "common.h"
#include <stdbool.h>
#include "list.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

struct tree
{
  struct node *root;
  element_copy_fun element_copy;
  key_free_fun key_free;
  element_free_fun element_free;
  element_comp_fun element_compare;
};

struct node
{
  elem_t name;
  elem_t elem; 
  struct node *left;
  struct node *right;
};

struct node *new_node(elem_t name, elem_t elem)
{
  struct node *temp = malloc(sizeof(struct node));
  temp->name = name;
  temp->elem = elem;
  temp->left = NULL;
  temp->right = NULL;
  return temp;
}

elem_t node_get_name(node_t *node)
{
  return node->name;
}

elem_t tree_get_name(tree_t *tree)
{
  return node_get_name(tree->root);
}


tree_t *tree_new(element_copy_fun element_copy, key_free_fun key_free, element_free_fun element_free, element_comp_fun element_compare)
{
  struct tree *temp = calloc(1,sizeof(struct tree));
  temp->root=NULL;
  temp->element_copy = element_copy;
  temp->key_free = key_free;
  temp->element_free = element_free;
  temp->element_compare = element_compare;
  return temp;
}

void free_element(element_free_fun fun, elem_t elem)
{
  if (fun != NULL)
    {
      ///Glöm inte listan av hyllor först
      fun(elem);
    }
}

void tree_delete_prim(tree_t *tree, struct node *n, bool delete_keys, bool delete_elements)
{
  if (n)
    {
      if (n->left)
        {
          tree_delete_prim(tree, n->left, delete_keys,  delete_elements);
        }
      if (n->right)
        {
          tree_delete_prim(tree, n->right, delete_keys, delete_elements);
        }
      if (delete_keys && tree->key_free)
        {
          tree->key_free(n->name);
        }
      if (delete_elements && tree->element_free)
        {
          tree->element_free(n->elem);
        }
      free(n);
    }
}

void tree_delete(tree_t *tree, bool delete_keys, bool delete_elements)
{
  if (tree)
    {
      tree_delete_prim(tree, tree->root, delete_keys, delete_elements);
    }
  free(tree);
}

int count_nodes(struct node *n)
{
  if (n)
    {
      return 1 + count_nodes(n->left) + count_nodes(n->right);
    }
  return 0;
}

void count_nodes_tail(struct node *n, int *sum)
{
  if (n)
    {
      *sum = *sum+1;
      count_nodes_tail(n->left,sum);
      count_nodes_tail(n->right,sum);
    }
}

int tree_size(tree_t *tree) 
{
  int result = 0;
  if (tree)
    {
       result = count_nodes(tree->root);
    }
  return result;
}

int bigger_number(int a, int b)
{
  if (a > b)
    {
      return a;
    }
  else
    {
      return b;
    }
}

int count_depth(struct node *n)
{
  if (n)
    {
      return 1 + bigger_number(count_depth(n->left),count_depth(n->right));
    }
  return 0;
}

int tree_depth(tree_t *tree)
{
  if (tree == NULL)
    {
      return 0;
    }
  else
    {
      return count_depth(tree->root);
    }
}

bool check_tree_for_string(struct node *nod, elem_t key, elem_t elem, element_comp_fun compare)
{
  if (nod)//Should always happen
    {
      if (compare(nod->name, key) == 0)
        {
          //Fråga bara om ny lagerplats och antal
          return false;
        }
      else if (compare(nod->name, key) > 0)
        {
          if (nod->left)
            {
              return check_tree_for_string(nod->left, key, elem, compare);
            }
          else
            {
              nod->left = new_node(key, elem);
              return true;
            }
        }
      else if (compare(nod->name, key) < 0)
        {
          if (nod->right)
            {
              return check_tree_for_string(nod->right, key, elem, compare);
            }
          else
            {
              nod->right = new_node(key, elem); 
              return true;
            }
        }
      else
        {
          return false;
        }
    }
  else
    {
      return false;
    }
  return false;
}
  
bool tree_insert(tree_t *tree, tree_key_t key, elem_t elem)
//Check if root exists
//check if the element is alphabetically before or after
//go down in tree accordingly (left for before, right for after)
{
  if (tree)
    {
      if (tree->root)
        {
          return check_tree_for_string(tree->root, key, elem, tree->element_compare);
        }
      else
        {
          tree->root = new_node(key,elem);
          return true;
        }
    }
  else
    {
      return false;
    }
}

//Optimize by checking alphabetic order, can be done by strcmp
bool check_tree_for_key(struct node *nod, elem_t key, element_comp_fun compare)//Just checks everything
{
  if (nod)
    {
      if (compare(nod->name,key)==0)
        {
          return true;
        }
      else if (check_tree_for_key(nod->left, key, compare) || check_tree_for_key(nod->right, key, compare))
        {
          return true;
        }
    }
  return false;
}

bool tree_has_key(tree_t *tree, tree_key_t key)
{
  if (tree && tree->root)
    {
      if (tree->root)
        {
          return check_tree_for_key(tree->root, key, tree->element_compare);
        }
      else
        {
          return false;
        }
    }
  return false;
}

elem_t *tree_get_prim(struct node *n, elem_t key, element_comp_fun compare)
{
  if (n)
    {
      if (compare(n->name, key) == 0)
        {
          return &n->elem;
        }
      if (compare(n->name, key) > 0)
        {
          if (n->left)
            {
              return tree_get_prim(n->left, key, compare);
            }
        }
      if (compare(n->name, key) < 0)
        {
          if (n->right)
            {
              return tree_get_prim(n->right, key, compare);
            }
        }
    }
  return NULL;
}

bool tree_get(tree_t *tree, tree_key_t key, elem_t *result)
{
  if (tree && tree_has_key(tree, key))
    {
      *result = *tree_get_prim(tree->root, key, tree->element_compare);
      return true;
    }
  else
    {
      return false;
    }
}

bool tree_remove(tree_t *tree, tree_key_t key, elem_t *result)
{
  struct node *n = tree->root;
  struct node *parent = NULL;

  while(1)
    {
      if (tree->element_compare(n->name, key))
        {
          if (!parent)
            {
              *result = n->elem;
              free(n);
              tree->root = NULL;
              return NULL;
            }
          else
            {
              if (tree->element_compare != NULL)
                {
                    if (tree->element_compare(parent->name, key) > 0)
                    {
                      *result = n->elem;
                      free(n);
                      parent->left = NULL;
                      return true;
                    }
                  else 
                    {
                      *result = n->elem;
                      free(n);
                      parent->right = NULL;
                      return true;
                    }
                }
              else
                {
                  char * treekey = key.p;
                  char * name = parent->name.p;
                  if (strcmp(name, treekey) > 0)
                    {
                      *result = n->elem;
                      free(n);
                      parent->left = NULL;
                      return true;
                    }
                  else 
                    {
                      *result = n->elem;
                      free(n);
                      parent->right = NULL;
                      return true;
                    }
                }
            }
        }
      else if (tree->element_compare != NULL)
        {
          if (tree->element_compare(n->name,key) > 0)
            {
              parent = n;
              n = n->left;
            }
          else
            {
              parent = n;
              n = n->right;
            }
        }
      else
        {
          char * treekey = key.p;
          char * name = n->name.p;
          if (strcmp(name, treekey) > 0)
            {
              parent = n;
              n = n->left;
            }
          else
            {
              parent = n;
              n = n->right;
            }
        }
    }
  return NULL;//endast för inlupp 1
}

elem_t *get_node_from_index_prim(struct node *n, int index, int *counter)
{
  elem_t *temp;
  if (n)
    {
      if (n->left)
        {
          temp = get_node_from_index_prim(n->left, index, counter);
          if (temp)
            {
              return temp;
            }
        }
      *counter = *counter +1;
      if (index == *counter)
        {
          return &n->elem;
        }
      if (n->right)
        {
          temp = get_node_from_index_prim(n->right, index, counter);
          if (temp)
            {
              return temp;
            }
        }
    }
  return NULL;
}

elem_t *get_node_from_index(tree_t *db, int index)
{
  int counter = 0;
  return get_node_from_index_prim(db->root, index, &counter);
}

void print_tree_ltr_prim(struct node *n, int *counter, int *page)
{
  if (n && *counter > 0)//noden finns
    {
      //Kör vänster trädet först
      print_tree_ltr_prim(n->left,counter,page);
      //Kör nuvarande noden
      printf("%d: %s\n",*counter,n->name.p);
      *counter = *counter +1;
      if (*counter%21==0)
        {
          char *val;
          do
            {
              puts("-------------------------------");
              val = ask_question_string("Visa fler varor(y,n)?");
            }
          while (val[0] != 'y' && val[0] != 'n');
          if (val[0] == 'n')
            {
              *counter = -1;
              return;
            }
          else
            {
              *page = *page +20;
              *counter = 1;
            }
        }
      //Kör högerträdet sen
      print_tree_ltr_prim(n->right, counter,page);
    }
  //noden fanns inte
}

int print_tree_ltr(tree_t *t)
{
  puts("-------------------------------");
  int counter = 1;
  int page = 0;
  if (t)
    {
      if (t->root)
        {
          print_tree_ltr_prim(t->root,&counter,&page);
        }
      else
        {
          //om trädet finns men är tomt
          printf("The tree is empty\n");
        }
    }
  else
    {
      //eventuellt felmeddelande
    }
  puts("-------------------------------");
  return page;
}

void tree_keys_prim(struct node *n, elem_t *names, int *counter)
{
  if (n->right)
    {
      tree_keys_prim(n->right, names, counter);
    }
  if (n->left == NULL)
    {
      names[*counter] = n->name;
    }
  else
    {
      tree_keys_prim(n->left, names, counter);
      names[*counter] = n->name;
      *counter += 1;
    }
}

tree_key_t *tree_keys(tree_t *tree)
{
  if (tree)
    {
      elem_t *names = calloc(tree_size(tree), sizeof(elem_t));
      int counter = 0;
      tree_keys_prim(tree->root, names, &counter);
      return names;
    }
  else
    {
      return NULL;
    }
}

void tree_elements_prim(struct node *n, elem_t *elems, int *counter)
{
  if (n->right)
    {
      tree_elements_prim(n->right, elems, counter);
    }
  if (n->left == NULL)
    {
      elems[*counter] = n->elem;
      *counter += 1;
    }
  else
    {
      tree_elements_prim(n->left, elems, counter);
      elems[*counter] = n->elem;
      *counter += 1;
    }
}

elem_t *tree_elements(tree_t *tree)
{
  if (tree)
    {
      elem_t *elems = calloc(tree_size(tree), sizeof(elem_t));
      int counter = 0;
      tree_elements_prim(tree->root, elems, &counter);
      return elems;
    }
  else
    {
      return NULL;
    }
}

bool tree_apply_prim(struct node *n, enum tree_order order, key_elem_apply_fun fun, void *data)
{
  if (n)
    {
      if (order == preorder)
        {
          tree_apply_prim(n->left, order, fun, data);
          fun(n->name, n->elem, data);
        }
      if (order == inorder)
        {
          tree_apply_prim(n->right, order, fun, data);
          fun(n->name, n->elem, data);
        }
      if (order == postorder)
        {
          fun(n->name, n->elem, data);
        }
    }
  return true;
}

bool tree_apply(tree_t *tree, enum tree_order order, key_elem_apply_fun fun, void *data)
{
  return tree_apply_prim(tree->root, order, fun, data);
}



//Not needed yet
/*
void print_tree_ltr_prim(struct node *n,int *counter)
{
  if (n)//noden finns
    {
      //Kör vänster trädet först
      print_tree_ltr_prim(n->left,counter);
      //Kör nuvarande noden
      printf("%d: %s\n",*counter,n->name);
      *counter = *counter +1;
      //Kör högerträdet sen
      print_tree_ltr_prim(n->right, counter);
    }
  //noden fanns inte
}
void print_tree_ltr(struct tree *t)
{
  int counter = 0;
  if (t)
    {
      if (t->root)
        {
          print_tree_ltr_prim(t->root,&counter);
        }
      else
        {
          //om trädet finns men är tomt
          printf("The tree is empty\n");
        }
    }
  else
    {
      //eventuellt felmeddelande
    }
}
*/
/*
int main()
{
  struct tree *t = tree_new();
  printf("trädet har: %d noder\n",tree_size(t));
  print_tree_ltr(t);
  tree_insert(t,"Apelsin", NULL);
  tree_insert(t,"Abaxoth", NULL);
  tree_insert(t,"Cthulu", NULL);
  tree_insert(t,"Baal", NULL);
  tree_insert(t,"Banan", NULL);
  tree_insert(t,"Citron", NULL);
  printf("trädet har: %d noder\n",tree_size(t));
  print_tree_ltr(t);
  printf("trädet är %d noder djupt\n",tree_depth(t));
  if (tree_has_key(t, "Citron"))
    {
      printf("Citron finns!\n");
    }
  printf("\n");
  printf("\n");
  printf("\n");
  tree_delete(t, free_element);
  t = NULL;
  printf("trädet är borttaget\n");
  
  
  printf("trädet har: %d noder\n",tree_size(t));
  
  return 0;
}
*/

///==================Not mandatory functions

