#include "list.h"
#include "shelf.h"
#include "tree.h"
#include <stdio.h>
#include <stdlib.h>

/*
KÄNDA PROBLEM:
kolla om L är en int fortfarande
 */

typedef elem_t L;

struct list
{
  struct link * first;
  struct link * last;
  element_copy_fun copy;
  element_free_fun free;
  element_comp_fun compare;
};

struct link
{
  L element;
  struct link *next;
};

list_t *list_new(element_copy_fun copy, element_free_fun free, element_comp_fun compare)
{
  list_t *emptylist = calloc(1,sizeof(list_t));
  emptylist->first = NULL;
  emptylist->last = NULL;
  emptylist->copy = copy;
  emptylist->free = free;
  emptylist->compare = compare;
  
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
  if (list->last)  //Om listan inte är tom
    {
      if (list->copy != NULL)
        {
          elem = list->copy(elem);
          list->last->next = link_new(NULL,elem);
          list->last = list->last->next;
        }
      else
        {
          list->last->next = link_new(NULL,elem);
          list->last = list->last->next;
        }
    }
  else
    {
      list_prepend(list,elem);
    }
}

void list_prepend(list_t *list, L elem)
{
  if (list->copy != NULL)
    {
      elem = list->copy(elem);
      list->first = link_new(list->first, elem);
      if (list->last == NULL)
        {
          list->last = list->first; 
        }
    }
  else
    {
      list->first = link_new(list->first, elem);
    }
}

int normalize_index(list_t *list, int index)
{
  int listLength = list_length(list); 
  return index = listLength + 1 + index;
}

void list_insert(list_t *list, int index, elem_t elem)
{
  int listLength = list_length(list);
  index = normalize_index(list,index);
  if (index == 0)
    {
      if (list->copy != NULL)
        {
          elem = list->copy(elem);
          list_prepend(list,elem);
        }
      else
        {
          list_prepend(list,elem);
        }
    }
  else if (index == listLength)
    {
       if (list->copy != NULL)
        {
          elem = list->copy(elem);
          list_append(list,elem);
        }
      else
        {
          list_append(list,elem);
        }
    }
  else if (index < listLength) 
    {
      struct link *iterator = list->first;
      for (int i = 1; i < index; i++)
        {
          iterator = iterator->next;
        }
      if (list->copy != NULL)
        {
          elem = list->copy(elem);
          iterator->next = link_new(iterator->next,elem);
        }
      else
        {
          iterator->next = link_new(iterator->next,elem);
        }
    }
}

void list_remove_prim (list_t *list, int index, bool delete)
{
  struct link *iterator = list->first;
  for (int i = 1; i < index; i++)//magisk siffra ett för man står redan på första
    {
      iterator = iterator->next;
    }
  struct link *temp = iterator;
  iterator = iterator->next->next; //iterator-next finns alltid
  temp->next = iterator;
  if (delete)
    {
      list->free(iterator->element); 
    }
  free(iterator);
}

void list_remove(list_t *list, int index, bool delete)
{
  int listLength = list_length(list);
  if (index == 0)
    {
      list->first = list->first->next;
    }
  if (index < listLength)
    {
      list_remove_prim(list, index, delete);
    }
  if (index < 0)
    {
      list_remove_prim(list, index, delete);
    }
  if (index > listLength)
    {
      list_remove_prim(list, index, delete);
    }
}

bool list_get(list_t *list, int index, elem_t *result)
{
  if (index == 0)
    {
      return list_first(list, result);
    }
  if (index == -1)
    {
      return list_last(list, result);
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
  else if(index > list_length(list))
    {
      return false;
    }
  else
    {
      *result = current->element;
      return true;
    }
}

bool list_first(list_t *list, elem_t *result)
{
  if (list->first)
    {
      *result = list->first->element;
      return true;
    }
  return false;
}

bool list_last(list_t *list, elem_t *result)
{
  if (list->last)
    {
      *result = list->last->element;
      return true;
    }    
  return false;
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

void print_lm(struct LM elem)
{
  printf("%c%d\n",elem.bokstav,elem.hyllnummer);
}

void list_delete(list_t *list, bool delete)
{
  int listLength = list_length(list);
  for(int i = 0 ; i < listLength ; ++i)
    {
      list_remove(list, 0, delete);
    }
  free(list);
}

bool list_apply(list_t *list, elem_apply_fun fun, void *data)
{
  bool result = false;
  struct link *cursor = list->first;
  while (cursor != NULL)
    {
      result = fun(cursor->element, data) || result;
      cursor = cursor->next;
    }
  return result;
}

int list_contains(list_t *list, elem_t elem)
{
  struct link *cursor = list->first;
  int listLength = list_length(list);
  for(int i = 0 ; i < listLength ; ++i)
    {
      if (list->compare(elem, cursor->element))
        {
          return i;
        }
      cursor = cursor->next;
    }
  return -1;
}

struct list *copy_list(struct list *l)
{
  int length = list_length(l);
  struct link *link = l->first;
  struct list *temp = list_new(l->copy, l->free, l->compare);
  for (int i = 0; i<length; i++)
    {
      list_append(temp,link->element);
      link = link->next;
    }
  return temp;
}
