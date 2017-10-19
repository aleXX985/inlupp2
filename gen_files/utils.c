#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "utils.h"

void clean_input_buffer(){
   int c;
   do{
      c = getchar();
   }while (c != '\n' && c != EOF);
}

int read_string(char* buf, int buf_size){
   int c;
   int i = 0;
   do{
      c = getchar();
      if(i < buf_size-1 && c != '\n'){
         buf[i] = c;
         i++;
      }
   }while(c != '\n' && c != EOF);
   buf[i] = '\0';
   return i;
}

bool is_number(char *str){
   int len = strlen(str);
   bool num = false;
   for(int i = 0; i < len; i++){
      if(str[i] == '-'){
         if(i != 0){
            return false;
         }
      }else if (isdigit(str[i])){
         num = true;
      }else{
         return false;
      }
   }
   return num;
}

bool is_float(char *str){
   int len = strlen(str);
   int dot = false;
   bool num = false;
   for(int i = 0; i < len; i++){
      if(str[i] == '.'){
         if(dot){
            return false;
         }else{
            dot = true;
         }
      }else if(str[i] == '-'){
         if(i != 0){
            return false;
         }
      }else if(isdigit(str[i])){
         num = true;
      }else{
         return false;
      }
   }
   return num;
}

bool is_shelf(char *shelf)
{
  char first = shelf[0];
  return 'A' <= first && first <= 'Z' && is_number(shelf + 1);
}

bool is_str_not_empty(char* s){
   return strlen(s) > 0;
}

bool is_valid_input(char *menu)
{
  const char *valid_characters = "LKTRAGHlktragh";
  char *c = menu;
  if (strlen(menu) > 1)
    {
      return false;
    }
  else
    {
      if (strchr(valid_characters, *c))
        {
          return *c;
        }
      else
        {
          return false;
        }
    }
}

bool is_edit_choice(char *choice)
{
  const char *valid_characters = "BPLTAbplta";
  char *c = choice;
  if (strlen(choice) > 1)
    {
      return false;
    }
  else
    {
      if (strchr(valid_characters, *c))
        {
          return *c;
        }
      else
        {
          return false;
        }
    }
}


answer_t ask_question(char *question, check_func check, convert_func convert){

   char buf[255];
   do
     {
       printf("%s\n", question);
       read_string(buf, 255);
       if (!check(buf))
         {
           printf("Felaktig inmatning: '%s'\n", buf);
         }
     }
   while(!check(buf));

   return convert(buf);
}

int ask_question_int(char * question){
   return ask_question(question, is_number, (convert_func) atoi).i;
}

char* ask_question_string(char * question){
   return ask_question(question, is_str_not_empty, (convert_func) strdup).s;
}

float ask_question_float(char * question){
   return ask_question(question, is_float, (convert_func) atof).f;
}

char* ask_question_shelf(char * question)
{
  return ask_question(question, is_shelf, (convert_func) strdup).s;
}

char *ask_question_menu(char *question)
{
  return (ask_question(question, is_valid_input, (convert_func) strdup).s);
}

char *ask_question_edit(char *question)
{
  return (ask_question(question, is_edit_choice, (convert_func) strdup).s);
}

char* trim(char* str){
   int i = 0;
   int b = -1, e = 0;
   while(str[i] != '\0'){
      if(b == -1 && !isspace(str[i])){
         b = i;
      }
      if(!isspace(str[i])){
         e = i;
      }
      i++;
   }

   int newlen;
   if(b == -1){
      newlen = 1;
   }else{
      newlen = (e-b) + 2;
   }
   char* r = (char*)malloc(sizeof(char) * newlen);
   for(int i = 0; i < newlen-1; i++){
      r[i] = str[b+i];
   }
   r[newlen-1] = '\0';
   return r;
}
