#ifndef FIELDS_H_INCLUDED
#define FIELDS_H_INCLUDED

#define get_field(x, y) _get_field(x, y, __func__)

struct Field* _get_field(int x, int y, const char *caller);
int is_field_in_segment(struct Field *field);

void init_field(struct Field *field);
void move_field_to(struct Field *source_field, struct Field *target_field);
void copy_field_to(struct Field *source_field, struct Field *target_field);
void reset_field(struct Field *field);

int is_near_border(struct Field *field);
int is_border_field(struct Field *field);

#endif
