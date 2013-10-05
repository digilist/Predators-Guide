#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "config.h"
#include "helpers.h"
#include "map.h"
#include "simulation.h"
#include "bmp.h"
#include "parallel.h"

char abstract_bitmap_filepath[256];

void _init_segment();
void _init_fields();
void _init_population();

struct Map *_map = 0;
struct Segment *_segment = 0;

int _rows;
int _cols;
int _fields;

struct Map* get_map()
{
	return _map;
}

/**
 * get the segment, in which the simulation for this process should run
 */
struct Segment* get_segment()
{
	return _segment;
}

/**
 * returns the number of columns, the map is seperated devided in
 */
int get_cols()
{
	return _cols;
}

/**
 * returns the number of rows, the map is seperated devided in
 */
int get_rows()
{
	return _rows;
}

/**
 * 	Initialisiert das Spielfeld mit der angebenen Breite und Höhe
 *
 */
void init_map()
{
	if(_map != 0)
		return;

	sprintf(abstract_bitmap_filepath, "%s%s", SAVE_PATH, BITMAP_FILENAME);

	_map = malloc(sizeof(struct Map));
	_map->width = MAP_WIDTH;
	_map->height = MAP_HEIGHT;

	_init_segment();
	_init_fields();

	_init_population();
}

/**
 * init the segment for this process and save it to a local variable
 */
void _init_segment()
{
	struct Map *map = get_map();

	int num_simulators = get_num_processes();
	int rank = get_rank();

	_rows = 1;
	_cols = 1;

	if(num_simulators > 1)
	{
		// calculate number of segments, seperated in rows and columns
		// this algorithm is based on prime factorization
		int n = num_simulators;
		int div = 2;
		int primes[10]; // under the assumption, there are max. 10 prime factors in the domain of n / num_simulators
		int p = 0; // highest index in primes

		while(n % div != 0)
			div++;

		while(n % div == 0)
		{
			primes[p] = div;

			if(n == div && (p == 0 || 1)) // primes and end
			{
				break;
			}

			n = n / div;
			p++;

			while(n % div != 0)
				div++;
		}

		if (p == 0) // prime
		{
			primes[1] = 1;
			p++;
		}

		// reduce all prime factors to 2 values
		for(int i = 1; i < p; i++)
		{
			primes[0] *= primes[i];
			if(p - 1 > i)
			{
				primes[p - 1] *= primes[p];
				p--;
			}
		}
		primes[1] = primes[p];

		_cols = primes[0];
		_rows = primes[1];
	}

	if(rank == 0)
	{
		printf("Splitting Map into %d cols and %d rows\n", _cols, _rows);
	}

	_fields = _cols * _rows;

	int segment_width = map->width / _cols;
	int segment_height = map->height / _rows;

	_segment = malloc(sizeof(struct Segment));

	_segment->x1 = (rank % _cols) * segment_width;
	_segment->x2 = _segment->x1 + segment_width - 1;
	_segment->y1 = (rank / _cols) * segment_height;
	_segment->y2 = _segment->y1 + segment_height - 1;
	_segment->width = segment_width;
	_segment->height = segment_height;

	// for modulo calculations
	rank++;

	// if cols not devide width make the last column a little bigger
	if(map->width % _cols != 0)
	{
		// but only if this process simulates a segment in the last column
		if(rank % _cols == 0)
		{
			printf("asdfa %d\n", get_rank());
			int diff = map->width - _cols * segment_width;

			_segment->width += diff;
			_segment->x2 += diff;
		}
	}

	// if rows not devide height make the last column a little bigger
	if(rank > 0 &&  map->height % _rows != 0)
	{
		// but only if this process simulates a segment in the last row
		if(rank > (_rows-1) * _cols)
		{
			int diff = map->height - _rows * segment_height;

			_segment->height += diff;
			_segment->y2 += diff;
		}
	}

	printf("Process %d Segment: %d:%d x %d:%d \n", get_rank(), _segment->x1, _segment->x2, _segment->y1, _segment->y2);
}

/**
 * init all fields (allocate memory, set coordinates...)
 *
 */
void _init_fields()
{
	struct Map *map = get_map();
	struct Segment *segment = get_segment();

	int fields = (segment->width + 2) * (segment->height + 2);
	map->fields = calloc(fields, sizeof(struct Field));

	// init all fields which are directly in the current segment
	for (int i = 0; i < segment->width; i++)
	{
		int x = segment->x1 + i;

		for (int j = 0; j < segment->height; j++)
		{
			int y = segment->y1 + j;

			struct Field *field = get_field(x, y);
			init_field(field);
			field->x = x;
			field->y = y;
		}
	}

	// outer borders
	for (int i = -1 ; i < segment->width + 1; i++)
	{
		// upper border
		int x = (segment->x1 + i + map->width) % map->width;
		int y = (segment->y1 - 1 + map->height) % map->height;

		struct Field *field = get_field(x, y);
		init_field(field);
		field->x = x;
		field->y = y;

		// lower border
		y = (segment->y2 + 1 + map->height) % map->height;

		field = get_field(x, y);
		init_field(field);
		field->x = x;
		field->y = y;
	}

	for (int j = -1 ; j < segment->height + 1; j++)
	{
		// left border
		int x = (segment->x1 - 1 + map->width) % map->width;
		int y = (segment->y1 + j + map->height) % map->height;

		struct Field *field = get_field(x, y);
		init_field(field);
		field->x = x;
		field->y = y;

		// right border
		x = (segment->x2 + 1 + map->width) % map->width;

		field = get_field(x, y);
		init_field(field);
		field->x = x;
		field->y = y;
	}
}

/**
 * create the initial population on our map
 *
 */
void _init_population()
{
	struct Map *map = get_map();
	struct Segment *segment = get_segment();

	output("Initializing the segment %d:%dx%d:%d (%dx%d) on a %dx%d map\n", segment->x1, segment->x2, segment->y1, segment->y2, segment->width, segment->height, map->width, map->height);

	int counter[NUMBER_OF_POPULATION_TYPES];
	for (int i = 0; i < NUMBER_OF_POPULATION_TYPES; i++)
		counter[i] = 0;
	int plants = 0;

	int predator_rate = (MAP_FILL_RATE * PREDATOR_RATE) * 100;
	int prey_rate = (MAP_FILL_RATE * PREY_RATE) * 100;
	int plant_rate = PLANT_RATE * 100;

	for (int x = segment->x1; x <= segment->x2; x++)
	{
		for (int y = segment->y1; y <= segment->y2; y++)
		{
			struct Field *field = get_field(x, y);
			field->x = x;
			field->y = y;

			int pop = EMPTY;
			if(random_int(0, 99) < predator_rate)
			{
				pop = PREDATOR;
			}
			else if(random_int(0, 99) < prey_rate)
			{
				pop = PREY;
			}

			if(random_int(0, 99) < plant_rate)
			{
				field->contains_plant= 1;
				plants++;
			}

			counter[pop]++;

			if(pop != EMPTY)
			{
				field->age = rand() % ELDERLY_AGE[pop]; // RANDOM age
				field->energy = rand() % MAX_ENERGY; // RANDOM energy
			}

			field->population_type = pop; // RANDOM population Type
		}
	}

	exchange_border_fields();

	output("%d: Spawned %d Predators / %d Prey / %d Plants / %d Empty\n", get_rank(), counter[PREDATOR], counter[PREY], plants, counter[EMPTY]);
}

/**
 * gibt das Spielfeld in eine Bitmap mit dem angebenen Dateipfad aus
 *
 */
void print_bitmap(int step)
{
	if(!PRINTING_ENABLED)
		return;

	struct Map *map = get_map();

	char filepath[256];
	sprintf(filepath, abstract_bitmap_filepath, step);

	int width = map->width;
	int height = map->height;

	char *pixel_map;
	pixel_map = malloc(sizeof(char) * width * height * 3);

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			struct Field *field = get_field(i, j);

			int index = 3 * (j * width + i);
			pixel_map[index] = 0; // r
			pixel_map[index + 1] = 0; // g
			pixel_map[index + 2] = 0; // b

			if (field->population_type == PREDATOR)
				pixel_map[index] = 255; // predetaors are red
			else if (field->population_type == PREY)
				pixel_map[index + 2] = 255; // preys are blue
			else if (field->contains_plant)
				pixel_map[index + 1] = 255;
			else if(field->population_type == EMPTY)
			{
				pixel_map[index] = 255;
				pixel_map[index + 1] = 255;
				pixel_map[index + 2] = 255;
			}
		}
	}

	// scale image (remove in final version)
	int scaleFactor = SCALE_FACTOR;
	if(scaleFactor > 1 && width == height)
	{
		int scaled_width = scaleFactor * width;
		int scaled_height = scaleFactor * height;

		char *scaled_pixel_map;
		scaled_pixel_map = malloc(sizeof(char) * scaled_width * scaled_height * 3);

		int px, py;
		for(int i = 0; i < scaled_width; i++)
		{
			for(int j = 0; j < scaled_height; j++)
			{
				px = (int) (j * (1.0 / scaleFactor));
				py = (int) (i * (1.0 / scaleFactor));

				int index = 3 * (i * scaled_width + j);
				int origIndex = 3 * (py * width + px);

				scaled_pixel_map[index] = pixel_map[origIndex];
				scaled_pixel_map[index + 1] = pixel_map[origIndex + 1];
				scaled_pixel_map[index + 2] = pixel_map[origIndex + 2];
			}
		}

		write_bmp(filepath, scaled_width, scaled_height, scaled_pixel_map);

		free(scaled_pixel_map);
	}
	else
	{
		write_bmp(filepath, width, height, pixel_map);
	}

	free(pixel_map);

}
