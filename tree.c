#include "tree.h"
#include <stdbool.h>
#include "list.h"
//#include "list.c"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct tree
{
  struct node *root;
};

struct node
{
  K name;
  T elem; //T = struct vara*
  struct node *left;
  struct node *right;
};

struct vara
{
  K name;
  char *beskrivning;
  int pris;
  struct list *shelflist;
  int antal;
};
/*
T new_element(K namn,char *description,int price, struct list* lista, int amount)//lista malloceras?
{
  T temp = malloc(sizeof(struct vara));
  temp->name = namn;
  temp->beskrivning = description;
  temp->pris = price;
  temp->shelflist = lista;
  temp->antal = amount;
  return temp;
};
*/
struct node *new_node(K name, T elem)
{
  struct node *temp = malloc(sizeof(struct node));
  temp->name = name;
  temp->elem = elem;
  temp->left = NULL;
  temp->right = NULL;
  return temp;
}

tree_t *tree_new()
{
  struct tree *temp = calloc(1,sizeof(struct tree));
  temp->root=NULL;
  return temp;
}

void free_element(K key, T elem)//Varför ska den här ha key?
{
  if (elem)
    {
      ///Glöm inte listan av hyllor först
      free(elem->shelflist);
      free(elem);
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

bool check_tree_for_string(struct node *nod, K key,T elem)
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

int balans_faktor(struct node *nod) {
  int balans_faktor = 0;
  if(nod->right)
    {
      balans_faktor -= count_depth(nod->right);
    }
  if(nod->left)
    {
      balans_faktor += count_depth(nod->left);
    }
  return balans_faktor;
}

struct node *rotera_left_left(struct node *nod) {
  struct node *original_nod = nod;
  struct node *original_left = original_nod->left;
  original_nod->left = original_left->right;
  original_left->right = original_nod;
  return(original_left);
}

struct node *rotera_left_right(struct node *nod) {
  struct node *original_nod = nod;
  struct node *original_left = original_nod->left;
  struct node *left_right = original_left->right;
  original_nod->left = left_right->right;
  original_left->right = left_right->left; 
  left_right->left = original_left;
  left_right->right = original_nod;
  return(left_right);
}

struct node *rotera_right_left(struct node *nod) {
  struct node *original_nod = nod;
  struct node *original_right = original_nod->right;
  struct node *right_left = original_right->left;
  original_nod->right = right_left->left;
  original_right->left = right_left->right; 
  right_left->right = original_right;
  right_left->left = original_nod;
  return(right_left);
}

struct node *rotera_right_right(struct node *nod) {
  struct node *original_nod = nod;
  struct node *original_right = original_nod->right;
  original_nod->right = original_right->left;
  original_right->left = original_nod; 
  return(original_right);
}

struct node *balansera_nod(struct node *nod) {
  struct node *ny_rot = NULL;

  if(nod->left)
    {
      nod->left  = balansera_nod(nod->left);
    }
  if(nod->right)
    {
      nod->right = balansera_nod(nod->right);
    }

  int balans_faktor_original = balans_faktor(nod);

  if(balans_faktor_original <= -2)
    {
      if(balans_faktor(nod->right) >= 1)
	{
	  ny_rot = rotera_right_left(nod);
	}
      else
	{
	  ny_rot = rotera_right_right(nod);
	}

    }
  else if(balans_faktor_original >= 2)
    {
	
      if(balans_faktor(nod->left) <= -1)
	{
	  ny_rot = rotera_left_right(nod);
	}
      else
	{
	  ny_rot = rotera_left_left(nod);
	}

    }
  else
    {
      ny_rot = nod;
    }

  return(ny_rot);	
}

void balance(tree_t *tree)
{
	struct node *ny_rot = NULL;
	ny_rot = balansera_nod(tree->root);
	tree->root = ny_rot; 
}

bool tree_insert(tree_t *tree, K key, T elem)
//Check if root exists
//check if the element is alphabetically before or after
//go down in tree accordingly (left for before, right for after)
{
  if (tree)
    {
      if (tree->root)
        {
          bool temp = check_tree_for_string(tree->root, key, elem);
	  balance(tree);
	  return temp;
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
bool check_tree_for_key(struct node *nod, K key)//Just checks everything
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

bool tree_has_key(tree_t *tree, K key) //shouldn't need tail recursion, just to select the correct node path
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

T tree_get_prim(struct node *n, K key)
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

/// Returns the element for a given key in tree.
/// (The implementation may assume that the key exists, or reserve
/// e.g. NULL as a value to indicate failure)
/// 
/// \param tree pointer to the tree
/// \param key the key of elem to be removed
/// \returns: the element associated wity key key
T tree_get(tree_t *tree, K key)
{
  if (tree && tree_has_key(tree, key))
    {
      return tree_get_prim(tree->root, key);
    }
  return NULL;
}

T tree_remove(tree_t *tree, K key)
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

int collect_elements_old(struct node *node, T *elements, int cur_index)
{
  if (node->left == NULL && node->right == NULL)
    {
      // End of branch
      elements[cur_index] = node->elem;
      return cur_index + 1;
    }
  else
    {
      int new_index = cur_index;
      if (node->left != NULL)
	{
	  // Collect nodes in left branch first
	  new_index = collect_elements_old(node->left, elements, cur_index);
	}

      // Add this node after every node on the left branch has been added
      elements[new_index] = node->elem;
      
      if (node->right == NULL)
	{
	  // Return new index to parent node when done with this node
	  return new_index + 1;
	}
      else
	{
	  // Collect all nodes in right branch before parent node is collected
	  return collect_elements_old(node->right, elements, new_index + 1);
	}
    }
}

/// Returns an array holding all the elements in the tree
/// in ascending order of their keys (which are not part
/// of the value).
///
/// \param tree pointer to the tree
/// \returns: array of tree_size() elements
T *tree_elements(tree_t *tree)
{
  int size = tree_size(tree);
  T *elements = calloc(size, sizeof(T));
  if (size > 0)
    {
      collect_elements_old(tree->root, elements, 0);
    }
  return elements;
}

int collect_keys(struct node *node, K *keys, int cur_index)
{
  if (node->left == NULL && node->right == NULL)
    {
      // End of branch
      keys[cur_index] = node->name;
      return cur_index + 1;
    }
  else
    {
      int new_index = cur_index;
      if (node->left != NULL)
	{
	  // Collect nodes in left branch first
	  new_index = collect_keys(node->left, keys, cur_index);
	}

      // Add this node after every node on the left branch has been added
      keys[new_index] = node->name;
      
      if (node->right == NULL)
	{
	  // Return new index to parent node when done with this node
	  return new_index + 1;
	}
      else
	{
	  // Collect all nodes in right branch before parent node is collected
	  return collect_keys(node->right, keys, new_index + 1);
	}
    }
}

/// Returns an array holding all the keys in the tree
/// in ascending order.
///
/// \param tree pointer to the tree
/// \returns: array of tree_size() keys
K *tree_keys(tree_t *tree)
{
  int size = tree_size(tree);
  K *keys = calloc(size, sizeof(K));
  if (size > 0)
    {
      collect_keys(tree->root, keys, 0);
    }
  return keys;
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
