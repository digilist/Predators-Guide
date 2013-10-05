#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "helpers.h"
#include "map.h"
#include "fields.h"
#include "parallel.h"

/**
 * get a random number between (and including) low and high
 *
 */
int random_int(const int low, const int high)
{
	return low + (double)rand () * (high - low + 1) / RAND_MAX;
}

/**
 * shuffle the given list of elements
 *
 */
void shuffle(void **elements, int count)
{
	// shuffle
	for (int i = count - 1; i >= 0; i--)
	{
		int j = rand() % count;
		void *tmp = elements[j];
		elements[j] = elements[i];
		elements[i] = tmp;
	}
}

/**
 * like printf, but prints only if DEBUG=1
 *
 */
void output(const char* format, ...)
{
	if(!DEBUG) // disable for debugging
		return;

	va_list args;
	va_start(args, format);

	vprintf(format, args);

	va_end(args);
}

/**
 * print a single field with its information
 *
 */
void print_field(struct Field *field)
{
	struct Map *map = get_map();
	printf("%d: Field %dx%d, PopulationType: %d, Age: %d, Energy: %d, Contains Plant: %d (at address %p, offset %d)\n",
		get_rank(),
		field->x,
		field->y, field->population_type,
		field->age,
		field->energy,
		field->contains_plant,
		field,
		(int) (field - map->fields)
	);
}

/**
 * print all fields
 *
 */
void print_all_fields(int borders)
{
	struct Map *map = get_map();
	struct Segment *segment = get_segment();

	int fields = fields = segment->width * segment->height;
	int offset = 0;

	if(borders)
	{
		offset = fields;
		fields = (segment->width + 2 ) * (segment->height + 2);
	}

	for(; offset < fields; offset++)
	{
		struct Field *field = map->fields + offset;
		print_field(field);
	}
}
