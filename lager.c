#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "list.h"
#include "utils.h"
#include <string.h>

/*
SAKER ATT FIXA
ångra
Klart (kod 1) ångra lägga till nod 
Klart (kod 2) ångra lägga till hylla i lista
Klart (kod 3) ångra redigera beskrivning av vara
Klart (kod 4) ångra redigera pris av vara
Klart (kod 5) ångra ta bort hylla i lista/ångra redigera antal i hylla
 */
struct regret
{
  int code; //0 = ångrade, 1 = Lade till node
  struct node *nod;
  struct vara *vara;
  struct list *lista;
};

struct tree //Ska dessa finnas i denna .c eller i sina respektive .c?
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

struct vara //*T
{
  K name;
  char *beskrivning;
  int pris;
  struct list *shelflist;
};

struct LM
{
  char bokstav;
  int hyllnummer;
  int antal;
};

struct list
{
  struct link * first;
  struct link * last;
};

struct link
{
  L element;
  struct link *next;
};

T new_element(K namn,char *description,int price, L elem)//lista malloceras?
{
  T temp = calloc(1,sizeof(struct vara));
  temp->name = namn;
  temp->beskrivning = description;
  temp->pris = price;
  temp->shelflist = list_new();//Måste skapa en lista (Vafan peddan)
  list_append(temp->shelflist,elem); //Fakkade för att listan inte fanns
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

void print_tree_ltr_prim(struct node *n,int *counter, int *page)
{
  if (n && *counter > 0)//noden finns
    {
      //Kör vänster trädet först
      print_tree_ltr_prim(n->left,counter,page);
      //Kör nuvarande noden
      printf("%d: %s\n",*counter,n->name);
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

int print_tree_ltr(struct tree *t)
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

void free_elements(K key, T elem)//Varför ska den här ha key?
{
  if (elem)
    {
      ///Glöm inte listan av hyllor först
      free(elem->shelflist);
      free(elem);
    }
}

void does_node_have_shelf(struct node *n, char *shelf, bool *exists)
{
  if (n && n->elem && n->elem->shelflist) //om noden finns och elementet finns och elementet har en lista
    {
      //Optimeras genom att kolla om exists redan satts till true innan man kollar listan
      int length = list_length(n->elem->shelflist);
      if (length>0)//Den kan bara finnas in listan om listan har mer än noll objekt
        {
          struct list *temp = n->elem->shelflist;
          L *tle;
          for (int i = 0; i < length; i++)
            {
              tle = list_get(temp, i);
              if (shelf[0] == tle->bokstav)//Char kan direktkollas
                {
                  char *ptr;
                  long num = strtol(shelf+1,&ptr,10);
                  if (num == tle->hyllnummer)
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

void is_shelf_taken_prim(struct node* n, char *shelf, bool *exists)
{
  if (n)
    {
      //kan optimeras med att kolla om exists redan satts
      does_node_have_shelf(n,shelf,exists);
      is_shelf_taken_prim(n->left,shelf,exists);
      is_shelf_taken_prim(n->right,shelf,exists);
    }
}

bool is_shelf_taken(struct tree *db, char *shelf)
{
  bool isItThere = false;
  if (db && db->root)
    {
      is_shelf_taken_prim(db->root,shelf,&isItThere);
    }
  return isItThere;
}

void edit_element(T element, struct tree *db)
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
  L elem;
  elem.bokstav = shelf[0];
  char *ptr;
  long num = strtol(shelf+1,&ptr,10);
  elem.hyllnummer = num;
  //När vi har en giltig plats så 
  int amount = ask_question_int("Hur många?");
  elem.antal = amount;

  //element->name = name;
  element->beskrivning = desc;
  element->pris = price;
  element->shelflist = list_new();

  list_append(element->shelflist, elem);

    //free(name);
}

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

struct regret *new_regret(int code, struct node *nod, struct vara *vara, struct list *lista)
{
  struct regret *temp = calloc(1,sizeof(struct regret));
  temp->code = code;
  temp->nod = nod;
  temp->vara = vara;
  temp->lista = lista;
  return temp;
}

struct regret *new_item(struct tree *db)
{
  char *name = strdup(ask_question_string("Vad heter varan?"));
  //Kolla om varan redan finns
  bool exists = tree_has_key(db, name);
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
          L elem;
          elem.bokstav = shelf[0];
          char *ptr;
          long num = strtol(shelf+1,&ptr,10);
          elem.hyllnummer = num;
          //När vi har en giltig plats så 
          int amount = ask_question_int("Hur många?");
          elem.antal = amount;

          T tempelement = new_element(name, desc, price,elem);
          //Skapa ett temp element
          //fråga om vi vill ändra på temp elementet

          int val = 0;
          while (1)
            {
              val = new_item_menu();
              if (val == 1)
                {
                  tree_insert(db,name,tempelement);
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
          tree_insert(db,name,tempelement);///SKAPA ETT ELEMENT FRÅN SHELF STRÄNGEN
          //free(desc);
          //free(name);
          return new_regret(1,NULL,tempelement,NULL);
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
      
      L elem;
      elem.bokstav = shelf[0];
      char *ptr;
      long num = strtol(shelf+1,&ptr,10);
      elem.hyllnummer = num;
      //När vi har en giltig plats så 
      int amount = ask_question_int("Hur många?");
      elem.antal = amount;
      //Antar att vi kan sätta in hyllplatsen
      T temp = tree_get(db,name);
      if (list_length(temp->shelflist)== 0)
        {
          temp->shelflist = list_new();
        }
      list_append(temp->shelflist,elem);
      return new_regret(2,NULL,temp,NULL);
    }
}

T get_node_from_index_prim(struct node *n, int index, int *counter)
{
  T temp;
  if (n)
    {
      if (n->left)
        {
          temp = get_node_from_index_prim(n->left,index,counter);
          if (temp)
            {
              return temp;
            }
        }
      *counter = *counter +1;
      if (index == *counter)
        {
          return n->elem;
        }
      if (n->right)
        {
          temp = get_node_from_index_prim(n->right,index,counter);
          if (temp)
            {
              return temp;
            }
        }
    }
  return NULL;
}

T get_node_from_index(tree_t *db, int index)
{
  int counter = 0;
  return get_node_from_index_prim(db->root,index,&counter);
}

void print_edit(struct vara *temp)
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

void print_vara(T vara)
{
  printf("-------------------------------\n");
  printf("Namn: %s\n", vara->name);
  printf("Beskrivning: %s\n", vara->beskrivning);
  printf("Pris: %d\n", vara->pris);
  int listl = list_length(vara->shelflist);
  L *elem;
  for (int i = 0; i < listl; i++)
    {
      elem = list_get(vara->shelflist,i);
      printf("Hylla: %c%d Antal:%d\n",elem->bokstav,elem->hyllnummer,elem->antal);
    }
}
int list_contains(struct list *l, char *shelf)
{
  int length = list_length(l);
  L *temp;
  for (int i = 0; i < length; i++)
    {
      temp = list_get(l, i);
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

struct list *copy_list(struct list *l)
{
  int length = list_length(l);
  struct link *link = l->first;
  struct list *temp = list_new();
  for (int i = 0; i<length; i++)
    {
      list_append(temp,link->element);
      link = link->next;
    }
  return temp;
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
  T temp = get_node_from_index(db,val+page);
  T re = NULL;
  print_vara(temp);
  puts("-------------------------------");
  print_edit(temp);
  char *choice = ask_question_edit("Välj rad eller [a]vbryt:");
  if ((*choice == 'B') || (*choice == 'b'))
    {
      printf("\nNuvarande beskrivning: %s\n-------------------------------\n",temp->beskrivning);
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
              L elem;
              L *elemptr = &elem;
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
      int itemindex;
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
      T temp = get_node_from_index(db,itemindex+page);
      puts("");
      printf("%s finns lagrad på följande platser:\n",temp->name);
      int listl = list_length(temp->shelflist);
      L *elem;
      int counter = 0;
      for (int i = 0; i < listl; i++)
        {
          counter = counter + 1;
          elem = list_get(temp->shelflist,i);
          printf("%d: %c%d (%d stycken)\n",counter,elem->bokstav,elem->hyllnummer,elem->antal);
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
      T templist = get_node_from_index(db,shelfindex);
      if (shelfindex == 0)
        {
          return 0;
        }
      else 
        {
          list_remove(temp->shelflist, shelfindex - 1, NULL);
          if (list_length(temp->shelflist) == 0)
            {
              tree_remove(db, templist->name);
            }
        }
    }
  return 0;
}

void write_file(tree_t *db)
{
  FILE *fp;
  fp = fopen("db.txt", "w");
  //K *keys = tree_keys(db);
  T *elements = tree_elements(db);
  for(int i = 0; i < tree_depth(db); i++)
    {
      //char *name = keys[i];
      T item = elements[i];
      char *name = item->name;
      char *desc = item->beskrivning;
      int price = item->pris;
      fprintf(fp, "Namn %s \nBeskrivning %s \nPris %d\n", name, desc, price);
      struct list *shelves = item->shelflist;
      for(int i = 0; i < list_length(shelves); i++)
	{
	  L *temp_shelf = list_get(shelves, i);
	  char shelf_letter = temp_shelf->bokstav;
	  int shelf_number = temp_shelf->hyllnummer;
	  int shelf_amount = temp_shelf->antal;
	  fprintf(fp, "Hylla %d %d %d\n", shelf_letter, shelf_number, shelf_amount);
	}
      if(i+1 < tree_depth(db)) fprintf(fp, "next\n");
    }
  fprintf(fp, "EOF");
  fclose(fp);
}

void read_file(tree_t *db)
{
  char type[999];
  char value[999];
  int num;
  int num2;
  int amount;
  FILE *fp;
  fp = fopen("db.txt", "r");
  if(fp)
    {
      while(true)
	{
	  //printf("--%s--%s--\n", type, value);
	  if(strcmp(type, "EOF") == 0) break;
	  T temp = calloc(1, sizeof(struct vara));
	  fscanf(fp, "%s %s", type, value); //value = name
	  if(strcmp(type, "EOF") == 0) break;;
	  temp->name = strdup(value);
	  //printf("%s - %s\n", type, value);
	  fscanf(fp, "%s %s", type, value); //value = desc
	  temp->beskrivning = strdup(value);
	  //printf("%s - %s\n", type, value);
	  fscanf(fp, "%s %d", type, &num); //num = price
	  temp->pris = num;
	  //printf("%s - %d\n", type,  num);
	  temp->shelflist = list_new();
	  struct list *shelves = temp->shelflist;
	  while(1)
	    {
	      L elem;
	      //if(strcmp(type, "\n") == 0 || strcmp(type, "EOF") == 0) break;
	      fscanf(fp, "%s %d %d %d", type, &num, &num2, &amount);//num = hyllbokstav, num2 = hyllnummer, amount = hyllantal
	      if(strcmp(type, "next") == 0 || strcmp(type, "EOF") == 0) break;
	      elem.bokstav = num;
	      elem.hyllnummer = num2;
	      elem.antal = amount;
	      list_append(shelves, elem);
	      //printf("%s - %d - %d - %d\n", type, num, num2, amount);
	    }
	  tree_insert(db, temp->name, temp);
	}
      fclose(fp);
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
	  write_file(db);
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

void fyll_med_fejk(tree_t *db)
{
  char lista[27] = "ajklmnopqtuvxyzbcdefghirs\n";
  L elem;
  for (int i = 0; i < 22; i++)
    {
      elem.bokstav = 'B';
      elem.hyllnummer = i;
      elem.antal = i;
      struct vara *vara = new_element(*(&lista+i),"1",1,elem);//FULKOD
      tree_insert(db,vara->name,vara);
    }
  
}

bool check_file()
{
  FILE *fp;
  char temp[999];
  fp = fopen("db.txt", "r");
  if(fscanf(fp, "%s", temp) == EOF) return false;
  return true;
}

int main(void)
{
  tree_t *db = tree_new();//Skapa databasträdet
  if(check_file()) read_file(db);
    //fyll_med_fejk(db);
  event_loop(db);
  printf("databasen hade %d noder.\n",tree_size(db));
  tree_delete(db,free_elements);//frigör allt minne innan avslut
  return 0;
}
