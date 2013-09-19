#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "config.h"
#include "map.h"
#include "simulation.h"
#include "bmp.h"
#include "parallel.h"

char abstract_bitmap_filepath[256];

/**
 * 	Initialisiert das Spielfeld mit der angebenen Breite und Höhe
 *
 */
struct Map* init_map()
{
	sprintf(abstract_bitmap_filepath, "%s%s", SAVE_PATH, BITMAP_FILENAME);

	struct Map *map = malloc(sizeof(struct Map));
	map->width = MAP_WIDTH;
	map->height = MAP_HEIGHT;

	map->fields = calloc(map->width * map->height, sizeof(struct Field));

	return map;
}

/**
 * create the initial population on our map
 *
 */
void init_population(struct Map *map)
{
	struct Segment *segment = get_segment();
	printf("Initializing the segment %d:%dx%d:%d (%dx%d) on a %dx%d field\n", segment->x1, segment->x2, segment->y1, segment->y2, segment->width, segment->height, map->width, map->height);

	int counter[NUMBER_OF_POPULATION_TYPES];
	for (int i = 0; i < NUMBER_OF_POPULATION_TYPES; i++)
		counter[i] = 0;
	int plants = 0;

	int predator_rate = (MAP_FILL_RATE * PREDATOR_RATE) * 100;
	int prey_rate = (MAP_FILL_RATE * PREY_RATE) * 100;
	int plant_rate = PLANT_RATE * 100;

	for (int i = segment->x1; i <= segment->x2; i++)
	{
		for (int j = segment->y1; j <= segment->y2; j++)
		{
			struct Field *field = get_field(map, i, j);
			reset_field(field);

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
				field->age = rand() % ELDERLY_AGE[pop]; // RANDOM age

			field->population_type = pop; // RANDOM population Type
			field->x = i;
			field->y = j;
		}
	}

	printf("Spawned %d Prey\n", counter[PREY]);
	printf("        %d Predators\n", counter[PREDATOR]);
	printf("        %d Plants\n", plants);
	printf("%d fields left Empty\n", counter[EMPTY]);
}

/**
 * gibt das Spielfeld in eine Bitmap mit dem angebenen Dateipfad aus
 *
 */
void print_bitmap(struct Map *map, int step)
{
	if(!PRINTING_ENABLED)
		return;

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
			struct Field *field = get_field(map, i, j);

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

/**
 * Gibt ein einzelnes Feld auf der Karte zurück
 *
 */
struct Field* get_field(struct Map *map, int x, int y)
{
	return map->fields + (y * map->width + x);
}

/**
 * kopiert ein Feld auf ein anderes Feld
 *
 */
void copy_field_to(struct Field *sourceField, struct Field *target_field)
{
	target_field->population_type = sourceField->population_type;
	target_field->age = sourceField->age;
	target_field->energy = sourceField->energy;
	target_field->last_step = sourceField->last_step;

	send_field_if_border(target_field);
}

/**
 * verschiebt ein Feld auf das Zielfeld
 * und schreibt die neue Adresse des Zielfeldes auf das Sourcefeld
 */
void move_field_to(struct Field **source_field, struct Field *target_field)
{
	copy_field_to(*source_field, target_field);
	reset_field(*source_field);

	*source_field = target_field;
}

/**
 * setzt ein Feld zurück, so dass dort kein Lebewesen mehr existiert
 *
 */
void reset_field(struct Field *field)
{
	field->population_type = EMPTY;
	field->age = 0;
	field->energy = 0;
	field->last_step = 0;
	field->contains_plant = 0;

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
	if(segment->x1 == field->x)
		return LEFT;
	if(segment->x2 == field->x)
		return RIGHT;

	// vertical
	if(segment->y1 == field->y)
		return UP;
	if(segment->y2 == field->y)
		return DOWN;

	return -1;
}
