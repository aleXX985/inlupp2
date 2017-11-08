#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tree.h"
#include "list.h"
#include "utils.h"
#include "shelf.h"
#include <string.h>
#include "item.h"
#include "lager.h"

struct item
{
  char *name;
  char *desc;
  int price;
  list_t *shelflist;
};

struct LM
{
  char bokstav;
  int hyllnummer;
  int antal;
};

int new_item_menu()
{
  char *choice;
  while (1)
    {
      choice = strdup(ask_question_string("Vill du lägga till i databasen? ([J]a, [N]ej, [R]edigera)"));
      if ((choice[0] == 'J') || (choice[0] == 'j'))
        {
          free(choice);
          return 1;
        }
      if ((choice[0] == 'N') || (choice[0] == 'n'))
        {
          free(choice);
          return 2;
        }
      if ((choice[0] == 'R') || (choice[0] == 'r'))
        {
          free(choice);
          return 3;
        }
      else
        {
          puts("Fel inmatning! Skriv in igen!");
        }
    }    
  return 0;
}

void edit_element(elem_t element, struct tree *db)
{
  //char *name = strdup(ask_question_string("Vad heter varan?"));
  char *desc = ask_question_string("Beskriv varan:");
  int price = ask_question_int("Vad kostar varan?");

  //Fortsätt fråga tills vi har en giltig plats
  char *shelf;
  bool taken = true;
  do
    {
      shelf = ask_question_shelf("Vilken hylla ligger varan på?");
      taken = is_shelf_taken(db,shelf);
      if (taken)
        {
          printf("\nHyllplatsen är redan tagen, välj en annan!\n");
        }
    }
  while (is_shelf_taken(db,shelf));
  elem_t elem;
  ((struct LM*)(elem.p))->bokstav = shelf[0];
  char *ptr;
  long num = strtol(shelf+1,&ptr,10);
  ((struct LM*)(elem.p))->hyllnummer = num;
  //När vi har en giltig plats så 
  int amount = ask_question_int("Hur många?");
  ((struct LM*)(elem.p))->antal = amount;

  //element->name = name;
  item_t *temp = element.p;
  temp->desc = desc;
  temp->price = price;
  temp->shelflist = list_new(NULL, NULL, NULL);

  list_append(temp->shelflist, elem);

    //free(name);
}

struct regret *new_item(struct tree *db)
{
  char *name = strdup(ask_question_string("Vad heter varan?"));
  tree_key_t temp_key;
  temp_key.p = name;
  //Kolla om varan redan finns
  bool exists = tree_has_key(db, temp_key);
  if (!exists)
    {
          char *desc = ask_question_string("Beskriv varan:");
          int price = ask_question_int("Vad kostar varan?");

          //Fortsätt fråga tills vi har en giltig plats
          char *shelf;
          bool taken = true;
          do
            {
              shelf = ask_question_shelf("Vilken hylla ligger varan på?");
              taken = is_shelf_taken(db,shelf);
              if (taken)
                {
                  printf("\nHyllplatsen är redan tagen, välj en annan!\n");
                }
            }
          while (is_shelf_taken(db,shelf));
          elem_t elem;
          ((struct LM*)(elem.p))->bokstav = shelf[0];
          char *ptr;
          long num = strtol(shelf+1,&ptr,10);
          ((struct LM*)(elem.p))->hyllnummer = num;
          //När vi har en giltig plats så 
          int amount = ask_question_int("Hur många?");
          ((struct LM*)(elem.p))->antal = amount;

          elem_t tempelement;
          tempelement.p = new_element(name, desc, price, elem.p);
          //Skapa ett temp element
          //fråga om vi vill ändra på temp elementet

          int val = 0;
          while (1)
            {
              val = new_item_menu();
              if (val == 1)
                {
                  tree_insert(db, temp_key,  tempelement);
                  break;
                }
              if (val == 2)
                {
                  //frigör tempelement?
                  return NULL;
                  break;
                  //inget ska hända
                }
              if (val == 3)
                {
                  edit_element(tempelement, db);
                }
            }
          tree_insert(db, temp_key ,tempelement);///SKAPA ETT ELEMENT FRÅN SHELF STRÄNGEN
          //free(desc);
          //free(name);
          return new_regret(1, NULL, &tempelement, NULL);
    }
  else
    {
      printf("Varan finns redan i systemet, använder samma beskrivning och pris\n");
      
      char *shelf;
      bool taken;
      do
        {
          shelf = ask_question_shelf("Vilken hylla ligger varan på?");
          taken = is_shelf_taken(db,shelf);
          if (taken)
            {
              printf("\nHyllplatsen är redan tagen, välj en annan!\n");
            }
        }
      while (is_shelf_taken(db,shelf));
      
      elem_t elem;
      ((struct LM*)(elem.p))->bokstav = shelf[0];
      char *ptr;
      long num = strtol(shelf+1,&ptr,10);
      ((struct LM*)(elem.p))->hyllnummer = num;
      //När vi har en giltig plats så 
      int amount = ask_question_int("Hur många?");
      ((struct LM*)(elem.p))->antal = amount;
      //Antar att vi kan sätta in hyllplatsen
      elem_t *result = NULL;
      tree_get(db, temp_key, result);
      item_t *temp_item = result->p;
      if (list_length(temp_item->shelflist)== 0)
        {
          temp_item->shelflist = list_new(NULL, NULL, NULL);
        }
      list_append(temp_item->shelflist,elem);
      return new_regret(2,NULL,result,NULL);
    }
}

item_t *create_element(char *namn, char *desc, int price, list_t *shelflist)
{
  item_t *temp = calloc(1,sizeof(struct item));
  temp->name = namn;
  temp->desc = desc;
  temp->price = price;
  temp->shelflist = shelflist;
  return temp;
};

void *delete_item(struct tree *db)
{
  puts("Ta bort vara");
  puts("============");
  puts("");
  int page = print_tree_ltr(db);
  if (tree_depth(db) == 0)
    {
      puts("Det finns inget att ta bort");
      return 0;
    }
  else
    {
      int itemindex = 0;
      do
        {
          puts("");
          itemindex = ask_question_int("Välj en vara att ta bort:");
          if ((itemindex + page > tree_size(db)) || (itemindex + page < 1))
            {
              printf("Felaktigt index! Välj ett nytt:\n");
            }
        }
      while ((itemindex + page > tree_size(db)) || (itemindex + page < 1));
      elem_t *temp = get_node_from_index(db,itemindex+page);
      item_t *temp_item = temp->p;
      puts("");
      printf("%s finns lagrad på följande platser:\n", temp_item->name);
      int listl = list_length(temp_item->shelflist);
      elem_t *elem = NULL;
      int counter = 0;
      for (int i = 0; i < listl; i++)
        {
          counter = counter + 1;
          list_get(temp_item->shelflist, i, elem);
          printf("%d: %c%d (%d stycken)\n", counter,  ((struct LM*)(elem->p))->bokstav,
                                                      ((struct LM*)(elem->p))->hyllnummer,
                                                      ((struct LM*)(elem->p))->antal);
        }
      puts("");
      int shelfindex;
      do
        {
          puts("");
          shelfindex = ask_question_int("Vilken plats skall tas bort (0 för ingen)?");
          if ((shelfindex > counter) || (shelfindex < 0))
            {
              printf("Felaktigt index! Välj ett nytt:\n");
            }
        }
      while ((shelfindex < 0) || (shelfindex > counter));
      elem_t *templist = get_node_from_index(db,shelfindex);
      temp_item = templist->p;
      if (shelfindex == 0)
        {
          return 0;
        }
      else 
        {
          list_remove(temp_item->shelflist, shelfindex - 1, NULL);
          if (list_length(temp_item->shelflist) == 0)
            {
              tree_key_t temp_key;
              temp_key.p = temp_item->name;
              tree_remove(db, temp_key, NULL);
            }
        }
    }
  return 0;
}

struct regret *edit_item(tree_t *db)
{
  struct regret *regret = new_regret(0,NULL,NULL,NULL);
  int val;
  int page = print_tree_ltr(db);
  do
    {
      val = ask_question_int("Vilken vara vill du redigera?");
      if ((val+page > tree_size(db)) || (val+page < 1))
        {
          printf("Felaktigt index! Välj ett nytt:\n");
        }
    }
  while ((val+page > tree_size(db)) || (val+page < 1));
  elem_t *temp = get_node_from_index(db,val+page);
  struct vara *temp_vara = temp->p;
  elem_t *re = NULL;
  print_vara(*temp);
  puts("-------------------------------");
  print_edit(temp->p);
  char *choice = ask_question_edit("Välj rad eller [a]vbryt:");
  if ((*choice == 'B') || (*choice == 'b'))
    {
      printf("\nNuvarande beskrivning: %s\n-------------------------------\n",temp_vara->beskrivning);
      char *beskrivning = ask_question_string("Skriv en ny beskrivning:");
      //free(temp->beskrivning);
      re = new_element(temp->name,temp->beskrivning,0,*list_get(temp->shelflist,0));///FIX
      regret->code = 3;
      regret->vara = re;
      temp->beskrivning = strdup(beskrivning);
    }
  if ((*choice == 'P') || (*choice == 'p'))
    {
      printf("\nNuvarande pris: %d\n-------------------------------\n",temp->pris);
      int pris = ask_question_int("Skriv in ett nytt pris:");
      re = new_element(temp->name,"",temp->pris,*list_get(temp->shelflist,0));///FIX
      regret->code = 4;
      regret->vara = re;
      temp->pris = pris;
    }
  if ((*choice == 'L') || (*choice == 'l'))
    {
      int finns = -1;
      while (finns == -1)
        {
          char *hylla = ask_question_shelf("Vilken hylla vill du ändra på?");
          finns = list_contains(temp->shelflist, hylla);
          if (finns != -1)
            {
              elem_t elem;
              elem_t *elemptr = &elem;
              int antal = ask_question_int("Skriv nytt antal");
              if (antal == 0)
                {
                  regret->lista = copy_list(temp->shelflist);
                  regret->vara = temp;
                  list_remove(temp->shelflist, finns, &elem);
                  
                  //elem innehåller det borttagna elementet.
                }
              else
                {
                  regret->lista = copy_list(temp->shelflist);
                  regret->vara = temp;
                  elemptr = list_get(temp->shelflist, finns);
                  elemptr->antal = antal;
                }
              regret->code = 5;
            }
        }
     
    }
  if ((*choice == 'A') || (*choice == 'a'))
    {
      //ska inte göra något
      free(regret);
      regret = NULL;
    }
  free(choice);
  return regret;
}

void free_shelflist(elem_t *elem)
{
  item_t *tempitem = elem->p;
  free(tempitem->shelflist);
}
