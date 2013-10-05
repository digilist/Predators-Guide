#include "config.h"

#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

/**
 * represents the map
 *
 */
struct Map
{
	unsigned int width : 16;
	unsigned int height : 16;

	struct Field *fields; // Belegung der einzelnen Felder des Spielfelds
	unsigned int numberOfPredators; // Anzahl Räuber aktuell
	unsigned int numberOfPrey; // Anzahl Beute aktuell
};

/**
 * represents a segment of the map, which will be simulated by one process
 *
 */
struct Segment {
	unsigned int x1 : 16;
	unsigned int x2 : 16;
	unsigned int y1 : 16;
	unsigned int y2 : 16;
	unsigned int width : 16;
	unsigned int height : 16;
};

/**
 * die different population types for one field: empty, prey and predator
 *
 */
enum PopulationType
{
	EMPTY,
	PREY,
	PREDATOR,
	NUMBER_OF_POPULATION_TYPES
};

struct Field
{
	 // coordinates of this field
	unsigned int x;
	unsigned int y;

	enum PopulationType population_type;

	int last_step; // in which step was the last movement of the animal on this field?

	// the following two values are just for predators and prey
	unsigned int age;
	int energy;


	int contains_plant; // boolean, whether there are plants on this field

};

void init_map();
struct Map* get_map();

struct Segment* get_segment();
int get_cols();
int get_rows();

void print_bitmap(int step);

#endif
