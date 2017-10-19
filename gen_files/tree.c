#include "tree.h"
#include "common.h"
#include <stdbool.h>
#include "list.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
  elem_t elem; //T = struct vara*
  struct node *left;
  struct node *right;
};

struct vara
{
  elem_t name;
  char *beskrivning;
  int pris;
  struct list *shelflist;
  int antal;
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

void free_element(element_free_fun fun, elem_t elem)//Varför ska den här ha key?
{
  if (fun != NULL)
    {
      ///Glöm inte listan av hyllor först
      fun(elem);
    }
}

void tree_delete_prim(struct node *n, tree_action cleanup)
{
  if (n)
    {
      if (n->left || n->right)//Kollar om noden har barn
        {
          if (n->left)
            {
              tree_delete_prim(n->left, cleanup);
            }
          if (n->right)
            {
              tree_delete_prim(n->right, free_element);
            }
        }
      free(n);
    }
}

void tree_delete(tree_t *tree, tree_action cleanup)//kolla hur listorna i varorna ska hanteras med free osv
{
  if (tree)
    {
      tree_delete_prim(tree->root, cleanup);
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

int tree_size(tree_t *tree) // fix a tail recursive version
{
  if (tree)
    {
      return count_nodes(tree->root);
    }
  return 0;
  /*
  int temp = 0;
  if (tree)
    {
      count_nodes_tail(tree->root,&temp);
    }
    return temp;*/
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
//int tree_depth(tree_t *tree);
//return 0 if null
//return 1 if exist
//return max of the two branches

bool check_tree_for_string(struct node *nod, elem_t key, elem_t elem)
{
  if (nod)//Should always happen
    {
      if (strcmp(nod->name, key) == 0)
        {
          //Fråga bara om ny lagerplats och antal
          return false;
        }
      else if (strcmp(nod->name, key) > 0)
        {
          if (nod->left)
            {
              return check_tree_for_string(nod->left, key, elem);
            }
          else
            {
              nod->left = new_node(key, elem);
              return true;
            }
        }
      else if (strcmp(nod->name, key) < 0)
        {
          if (nod->right)
            {
              return check_tree_for_string(nod->right, key, elem);
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
  
bool tree_insert(tree_t *tree, elem_t key, elem_t elem)
//Check if root exists
//check if the element is alphabetically before or after
//go down in tree accordingly (left for before, right for after)
{
  if (tree)
    {
      if (tree->root)
        {
          return check_tree_for_string(tree->root, key, elem);
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
bool check_tree_for_key(struct node *nod, elem_t key)//Just checks everything
{
  if (nod)
    {
      if (strcmp(nod->name,key)==0)
        {
          return true;
        }
      else if (check_tree_for_key(nod->left,key) || check_tree_for_key(nod->right,key))
        {
          return true;
        }
    }
  return false;
}

bool tree_has_key(tree_t *tree, elem_t key) //shouldn't need tail recursion, just to select the correct node path
{
  if (tree && tree->root)
    {
      if (tree->root)
        {
          return check_tree_for_key(tree->root, key);
        }
      else
        {
          return false;
        }
    }
  return false;
}

elem_t tree_get_prim(struct node *n, elem_t key)
{
  if (n)
    {
      if (strcmp(n->name, key) == 0)
        {
          return n->elem;
        }
      if (strcmp(n->name, key) > 0)
        {
          if (n->left)
            {
              return tree_get_prim(n->left, key);
            }
        }
      if (strcmp(n->name, key) < 0)
        {
          if (n->right)
            {
              return tree_get_prim(n->right, key);
            }
        }
    }
  return NULL;
}

elem_t tree_get(tree_t *tree, elem_t key)
{
  if (tree && tree_has_key(tree, key))
    {
      return tree_get_prim(tree->root, key);
    }
  return NULL;
}

elem_t tree_remove(tree_t *tree, elem_t key)
{
  //T temp;
  struct node *n = tree->root;
  struct node *parent = NULL;

  while(1)
    {
      if (key == n->name)
        {
          if (!parent)
            {
              free(n);
              tree->root = NULL;
              return NULL;
            }
          else
            {
              if (strcmp(parent->name, key) > 0)
                {
                  free(n);
                  parent->left = NULL;
                  return NULL;
                }
              else
                {
                  free(n);
                  parent->right = NULL;
                  return NULL;
                }
            }
        }
      else if (strcmp(n->name,key) > 0)
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
  
  return NULL;//endast för inlupp 1
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
