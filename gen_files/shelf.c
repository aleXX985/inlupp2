#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tree.h"
#include "list.h"
#include "utils.h"
#include "shelf.h"
#include <string.h>
#include "item.h"


struct LM
{
  char bokstav;
  int hyllnummer;
  int antal;
};

void does_node_have_shelf(node_t *n, char *shelf, bool *exists)
{
  list_t *shelves = ((item_t*)(n->elem.p))->shelflist;
  if (shelves) //om noden finns och elementet finns och elementet har en lista
    {
      //Optimeras genom att kolla om exists redan satts till true innan man kollar listan
      int length = list_length(shelves);
      if (length>0)//Den kan bara finnas in listan om listan har mer än noll objekt
        {
          struct list *temp = shelves;
          elem_t tle;
          for (int i = 0; i < length; i++)
            {
              list_get(temp, i, &tle);
              if (shelf[0] == (((struct LM*)(tle.p))->bokstav))//Char kan direktkollas
                {
                  char *ptr;
                  long num = strtol(shelf+1,&ptr,10);
                  if (num == ((struct LM*)(tle.p))->hyllnummer)
                    {
                      *exists = true;
                    }
                }
              //Kolla varje element i listan
              //Kolla först om bokstaven finns, kolla sen om siffran stämmer
              //Sätt exists till true om den hittas
            }
        }
    }
}

void is_shelf_taken_prim(node_t *n, char *shelf, bool *exists)
{
  if (n)
    {
      //kan optimeras med att kolla om exists redan satts
      does_node_have_shelf(n,shelf,exists);
      is_shelf_taken_prim(n->left,shelf,exists);
      is_shelf_taken_prim(n->right,shelf,exists);
    }
}

bool is_shelf_taken(tree_t *db, char *shelf)
{
  bool isItThere = false;
  if (db && db->root)
    {
      is_shelf_taken_prim(db->root,shelf,&isItThere);
    }
  return isItThere;
}

