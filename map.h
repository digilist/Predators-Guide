#include "config.h"

#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#define get_field(x, y) _get_field(x, y, __func__)

/**
 * Repräsentiert das Spielfeld
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
 * represents a segment of the map
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
 * die möglichen Feldbelegungen: leeres Feld, Räuber & Beute
 *
 * beim Hinzufügen oder Entfernen die Konfiguration in der config.h anpassen!
 */
enum PopulationType
{
	EMPTY,
	PREY,
	PREDATOR,
	NUMBER_OF_POPULATION_TYPES
};

/**
 * die Belegung eines Feldes
 *
 */
struct Field
{
	 // Position des Feldes
	unsigned int x;
	unsigned int y;

	enum PopulationType population_type;

	int last_step; // speichert, wann das Lebewesen seinen letzten Schritt vollzogen hat (um doppelte Schritte nach Bewegungen zu vermeiden)

	// wenn populationType == EMPTY muss keines dieser Werte gesetzt werden
	unsigned int age; // erreicht das Lebewesen ein bestimmtes Alter, bringt es ein neues Lebewesen auf die Welt

	unsigned int energy; // Anzahl der Zeiteinheiten, die das Tier hungern musste


	unsigned int contains_plant; // speicher, ob auf diesem Feld auch pflanzen existieren

};

void init_map();
struct Map* get_map();

struct Segment* get_segment();
int get_cols();
int get_rows();

struct Field* _get_field(int x, int y, const char *caller);
int is_field_in_segment(struct Field *field);

void copy_field_to(struct Field *source_field, struct Field *target_field);

void move_field_to(struct Field *source_field, struct Field *target_field);

void reset_field(struct Field *field);

int is_near_border(struct Field *field);
int is_border_field(struct Field *field);

void print_bitmap(int step);

#endif
