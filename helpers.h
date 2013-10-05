#ifndef HELPERS_H_INCLUDED
#define HELPERS_H_INCLUDED

#include "fields.h"

void shuffle(void **elements, int count);
int random_int(int low, int high);

void output(const char* format, ...);

void print_all_fields(int borders);
void print_field(struct Field *field);

#endif
