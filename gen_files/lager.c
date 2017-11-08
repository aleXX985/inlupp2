#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "list.h"
#include "utils.h"
#include "item.h"
#include "shelf.h"
#include <string.h>

/*
SAKER ATT FIXA
lager.c*/

struct regret
{
  int code; //0 = ångrade, 1 = Lade till node
  struct node *nod;
  elem_t *vara;
  struct list *lista;
};

struct vara
{
  elem_t name;
  char *beskrivning;
  int pris;
  struct list *shelflist;
  int antal;
};


item_t *new_element(char *namn, char *desc, int price, list_t *shelflist)
{
  item_t *temp = create_element(namn, desc, price, shelflist);
  return temp;
};


void print_menu()
{
  printf("===============================\n");
  printf("[L]ägga till en vara\n");
  printf("[T]a bort en vara\n");
  printf("[R]edigera en vara\n");
  printf("Ån[g]ra senaste ändring\n");
  printf("Lista [h]ela varukatalogen\n");
  printf("[K]ontrollera databasens sortering\n");
  printf("[A]vsluta\n");
  printf("===============================\n");
}

void free_elements(elem_t *name, elem_t *elem)
{
  if (name)
    {
      free_shelflist(elem);
      free(elem);
    }
}

void print_lager(tree_t *t)
{
  print_tree_ltr(t);
}

struct regret *new_regret(int code, struct node *nod, elem_t *vara, struct list *lista)
{
  struct regret *temp = calloc(1,sizeof(struct regret));
  temp->code = code;
  temp->nod = nod;
  temp->vara = vara;
  temp->lista = lista;
  return temp;
}

void print_edit(item_t *temp)
{
  puts("");
  puts("[B]eskrivning");
  puts("[P]ris");
  if (list_length(temp->shelflist))
    {
      puts("[L]agerhylla");
    }
  //puts("An[t]al");
  puts("");
}

void print_vara(elem_t vara)
{
  printf("-------------------------------\n");
  printf("Namn: %s\n", vara->name);
  printf("Beskrivning: %s\n", vara->beskrivning);
  printf("Pris: %d\n", vara->pris);
  int listl = list_length(vara->shelflist);
  elem_t *elem;
  for (int i = 0; i < listl; i++)
    {
      elem = list_get(vara->shelflist,i);
      printf("Hylla: %c%d Antal:%d\n",elem->bokstav,elem->hyllnummer,elem->antal);
    }
}
int list_contains(struct list *l, char *shelf)
{
  int length = list_length(l);
  elem_t *temp;
  for (int i = 0; i < length; i++)
    {
      temp = list_get(l, i, );
      if (shelf[0] == temp->bokstav)
        {
          char *ptr;
          long num = strtol(shelf + 1, &ptr, 10);
          if (num == temp->hyllnummer)
            {
              return i;
            }
        }
    }
  return -1;
}

void regret_action(struct regret *regret, struct tree *db)
{
  if (regret)
    {
      if (regret->code == 0)
        {
          printf("det finns inget att ångra!\n");
        }
      if (regret->code == 1)
        {
          //tree_remove returnerar ett T men det är oviktigt just nu
          tree_remove(db,regret->vara->name);
          regret->code = 0;
          regret->vara = NULL;
        }
      if (regret->code == 2)
        {
          list_remove(regret->vara->shelflist,list_length(regret->vara->shelflist)-1,NULL);
          regret->code = 0;
          regret->vara = NULL;
        }
      if (regret->code == 3)//ångra redigera beskrivning av vara
        {
          tree_get(db,regret->vara->name)->beskrivning = regret->vara->beskrivning;
          regret->code = 0;
          regret->vara = NULL;
        }
      if (regret->code == 4)//ångra redigera pris av vara
        {
          tree_get(db,regret->vara->name)->pris = regret->vara->pris;
          regret->code = 0;
          regret->vara = NULL;
        }
      if (regret->code == 5)//antal i hylla
        {
          tree_get(db,regret->vara->name)->shelflist = copy_list(regret->lista);
          regret->code = 0;
          regret->vara = NULL;
          //FREE LISTA?
          regret->lista = NULL;
        }
    }
}

int event_loop(tree_t *db)
{
  printf("\nVälkommen till lagerhantering 1.1\n");
  char *choice = NULL;
  struct regret *regret = NULL;
  struct regret *last = new_regret(0,NULL,NULL,NULL);
  //char *lastaction; //add, edit, 
  while (1)
    {
      print_menu();
      if (choice)
        {
          free(choice);
        }
      choice = ask_question_menu("\nVad vill du göra?");
      if (*choice == 'a' || *choice == 'A')
        {
          free(regret);
          free(last);
          free(choice);
          return 0;
        }
      if (*choice == 'l' || *choice == 'L')
        {
          regret = new_item(db);
          if (regret)
            {
              free(last);
              last = regret;
              regret = NULL;
            }
          ///Hantera fel?
        }
      if (*choice == 't' || *choice == 'T')
        {
          delete_item(db);
        }
      if (*choice == 'r' || *choice == 'R')
        {
          if (tree_size(db)>0)
            {
              regret = edit_item(db);
              if (regret)
                {
                  free(last);
                  last = regret;
                  regret = NULL;
                }
            }
          else
            {
              printf("Trädet är tomt, finns inget att redigera!\n");
            }
        }
      if (*choice == 'g' || *choice == 'G')
        {
          regret_action(last,db);
          //free regret, last?
        }
      if (*choice == 'h' || *choice == 'H')
        {
          print_tree_ltr(db);
        }
      if ((*choice == 'K') || (*choice == 'k'))
        {
          puts("Ej implementerat än.");
        }
    }
  return 1;
}

int main(void)
{
  tree_t *db = tree_new(element_copy, key_free, element_free, element_compare); //Skapa databasträdet
  //fyll_med_fejk(db);
  event_loop(db);
  printf("databasen hade %d noder.\n",tree_size(db));
  tree_delete(db, true, true); //frigör allt minne innan avslut
  return 0;
}
