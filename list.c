#include "list.h"
#include <stdio.h>
#include <stdlib.h>

/*
KÄNDA PROBLEM:
kolla om L är en int fortfarande
 */
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

list_t *list_new()
{
  list_t *emptylist = calloc(1,sizeof(list_t));
  emptylist->first = NULL;
  emptylist->last = NULL;
  return emptylist;
}
struct link *link_new(struct link *next, L elem)
{
  struct link *temp = malloc(sizeof(struct link));
  temp->next = next;
  temp->element = elem;
  return temp;
}

void list_append(list_t *list, L elem)
{
  if (list->last)//Om listan inte är tom
    {
      list->last->next = link_new(NULL,elem);
      list->last = list->last->next;
    }
  else
    {
      list_prepend(list,elem);
    }
}

void list_prepend(list_t *list, L elem)
{
  list->first = link_new(list->first, elem);
  if (list->last == NULL)
    {
      list->last = list->first;
    }
}

int normalize_index(list_t *list, int index)
{
  int listLength = list_length(list); 
  return index = listLength + 1 + index;
}

bool list_insert(list_t *list, int index, L elem)
{
  int listLength = list_length(list);
  index = normalize_index(list,index);
  if (index == 0)
    {
      list_prepend(list,elem);
      return true;
    }
  else if (index == listLength)
    {
      list_append(list,elem);
      return true;
    }
  else if (index < listLength) 
    {
      struct link *iterator = list->first;
      for (int i = 1; i < index; i++)
        {
          iterator = iterator->next;
        }
      iterator->next = link_new(iterator->next,elem);
      return true;
    }
  return false;
}

bool list_remove(list_t *list, int index, L *elem)//kan ej hantera negativa index
{
  int listLength = list_length(list);
  if (index == 0)
    {
      //Ta bort första linken
      //struct link *temp = list->first;
      elem = &list->first->element;
      list->first = list->first->next;
      //free(temp);
      return true;
    }
  if (index < listLength)
    {
      struct link *iterator = list->first;
      for (int i = 1; i < index; i++)//magisk siffra ett för man står redan på första
        {
          iterator = iterator->next;
        }
      struct link *temp = iterator;
      if(elem)
        {
          *elem = iterator->next->element;//sparar värdet
        }
      iterator = iterator->next->next; //iterator-next finns alltid
      temp->next = iterator;
    }
  return false;
}

L *list_get(list_t *list, int index)
{
  if (index == 0)
    {
      return list_first(list);
    }
  if (index == -1)
    {
      return list_last(list);
    }
  struct link *current = list->first;
  for (int i = 0;current && i < index ; i++)
    {
      current=current->next;
    }
  if (current == NULL)
    {
      return NULL;
    }
  else
    {
      return &current->element;
    }
}

L *list_first(list_t *list)
{
  if (list->first)
    {
      return &list->first->element;
    }
  return NULL;
}

L *list_last(list_t *list)
{
  if (list->last)
    {
      return &list->last->element;
    }
  return NULL;
}

int number_of_links_rek(struct link *link)
{
  if (link)//om link inte är NULL
    {
      return 1 + number_of_links_rek(link->next);
    }
  else
    {
      return 0;
    }
}

int number_of_links_ite(struct link *link)
{
  if (link)
    {
      int counter = 1;
      struct link *iterator = link;
      while (iterator->next)
        {
          counter++;
          iterator = iterator->next;
        }
      return counter;
    }
  else
    {
      return 0;
    }
}

int list_length(list_t *list)
{
  return number_of_links_ite(list->first);
}
void print_lm(L elem)
{
  printf("%c%d\n",elem.bokstav,elem.hyllnummer);
}
