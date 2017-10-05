#ifndef UTILS_H
#define UTILS_H
#include <stdbool.h>

typedef union
{
  int   i;
  float f;
  char *s;
  char c;
} answer_t;

typedef bool(*check_func)(char *);
typedef answer_t(*convert_func)(char *);

int read_string(char *buf, int buf_siz);
bool is_number(char *str);
bool is_shelf(char *shelf);
void clean_input_buffer();
int read_string(char* buf, int buf_size);
bool is_str_not_empty(char* s);
bool is_float(char *str);
bool is_edit_choice(char *choice);
int ask_question_int(char *question);
char* ask_question_string(char* question);
float ask_question_float(char * question);
char* ask_question_shelf(char *question);
char *ask_question_menu(char *qeustion);
char *ask_question_edit(char *question);

char* trim(char* str);
#endif
