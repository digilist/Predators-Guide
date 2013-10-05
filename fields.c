#include <stdio.h>
#include <stdlib.h>

#include "map.h"
#include "fields.h"
#include "simulation.h"
#include "parallel.h"


/**
 * is the given field in our segment?
 *
 */
int is_field_in_segment(struct Field *field)
{
	struct Segment *segment = get_segment();

	return field->x >= segment->x1 && field->x <= segment->x2
				&& field->y >= segment->y1 && field->y <= segment->y2;
}

/**
 * get a single field on the map at position (i|j)
 *
 */
struct Field* _get_field(int x, int y, const char *caller)
{
	struct Map *map = get_map();
	struct Segment *segment = get_segment();

	int i = x;
	int j = y;

	int offset = 0;

	/*****
	 * border means the first row or column out of this segment
	 *
	 * uuuuuuuuuuuuu
	 * l...........r
	 * l...........r
	 * l...........r
	 * bbbbbbbbbbbbb
	 *
	 * u = upper border
	 * l = left border
	 * r = right border
	 * b = lower bordr
	 */

	int upper_border = ((segment->y1 - 1 + map->height) % map->height) == y;
	int lower_border = ((segment->y2 + 1) % map->height) == y;
	int left_border = ((segment->x1 - 1 + map->width) % map->width) == x;
	int right_border = ((segment->x2 + 1) % map->width) == x;

	if(upper_border || lower_border || left_border || right_border)
	{
		offset = segment->width * segment->height;
	}
	else if(x < segment->x1 || x > segment->x2 || y < segment->y1 || y > segment->y2)
	{
		printf("%d: invalid field requested: %dx%d from %s\n", get_rank(), x, y, caller);
		exit(1);
	}

	if(upper_border)
	{
		// upper border after all other fields
		offset += 0;

		if(left_border)
		{
			i = segment->width;
		}
		else if(right_border)
		{
			i = segment->width + 1;
		}
		else
		{
			i -= segment->x1;
		}

		offset += i;
	}
	else if(lower_border)
	{
		// lower border fields after upper border fields
		offset += segment->width + 2;

		if(left_border)
		{
			i = segment->width;
		}
		else if(right_border)
		{
			i = segment->width + 1;
		}
		else
		{
			i -= segment->x1;
		}

		offset += i;
	}
	else if(left_border)
	{
		// left border fields after upper and lower border fields
		offset += (segment->width + 2) * 2;

		j -= segment->y1;
		offset += j;
	}
	else if(right_border)
	{
		// right border fields after upper, lower and left border fields
		offset += (segment->width + 2) * 2 + segment->height;

		j -= segment->y1;
		offset += j;
	}
	else
	{
		i -= segment->x1;
		j -= segment->y1;

		offset += j * segment->width + i;
	}
	struct Field *field = map->fields + offset;

	if(field->x > 0 && field->y > 0) // ignore init (and as a side effect, the zero point)
	{
		if(field->x != x || field->y != y)
		{
			printf("%d: mismatching field coordinates, requested %dx%d, found %dx%d, calculated offset %d (%d / %d), borders %d %d %d %d ... caller %s \n", get_rank(), x, y, field->x, field->y, offset, i, j, upper_border, lower_border, left_border, right_border, caller);
			exit(1);
		}
	}

	return field;
}

/**
 * copy a field into another field
 * if the target field is a border field,
 * it will notify the neighbored process
 *
 */
void copy_field_to(struct Field *source_field, struct Field *target_field)
{
	target_field->population_type = source_field->population_type;
	target_field->age = source_field->age;
	target_field->energy = source_field->energy;
	target_field->last_step = source_field->last_step;
}

/**
 * verschiebt ein Feld auf das Zielfeld
 * und schreibt die neue Adresse des Zielfeldes auf das Sourcefeld
 */
void move_field_to(struct Field *source_field, struct Field *target_field)
{
	copy_field_to(source_field, target_field);
	send_field(target_field);

	reset_field(source_field);
}

void init_field(struct Field *field)
{
	field->population_type = EMPTY;
	field->age = 0;
	field->energy = 0;
	field->last_step = 0;
	field->contains_plant = 0;
}

/**
 * reset the field (remove the animal)
 *
 */
void reset_field(struct Field *field)
{
	init_field(field);
	send_field_if_border(field);
}

/**
 * is the given field a border field?
 * if it is, it returns its direction
 * otherwise -1
 *
 */
int is_border_field(struct Field *field)
{
	struct Segment *segment = get_segment();

	// diagonal
	if(segment->x1 == field->x && segment->y1 == field->y)
		return UP_LEFT;
	if(segment->x1 == field->x && segment->y2 == field->y)
		return DOWN_LEFT;
	if(segment->x2 == field->x && segment->y1 == field->y)
		return UP_RIGHT;
	if(segment->x2 == field->x && segment->y2 == field->y)
		return DOWN_RIGHT;

	// horizontal
	if(segment->x1 == field->x && field->y >= segment->y1 && field->y <= segment->y2)
		return LEFT;
	if(segment->x2 == field->x && field->y >= segment->y1 && field->y <= segment->y2)
		return RIGHT;

	// vertical
	if(segment->y1 == field->y && field->x >= segment->x1 && field->x <= segment->x2)
		return UP;
	if(segment->y2 == field->y && field->x >= segment->x1 && field->x <= segment->x2)
		return DOWN;

	return -1;
}
