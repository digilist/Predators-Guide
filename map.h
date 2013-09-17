#include "config.h"

#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

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
	unsigned int x : 16;
	unsigned int y : 16;

	enum PopulationType population_type : 2;

	unsigned int lastStep : 16; // speichert, wann das Lebewesen seinen letzten Schritt vollzogen hat (um doppelte Schritte nach Bewegungen zu vermeiden)

	// wenn populationType == EMPTY muss keines dieser Werte gesetzt werden
	unsigned int age : 8; // erreicht das Lebewesen ein bestimmtes Alter, bringt es ein neues Lebewesen auf die Welt

	unsigned int energy : 8; // Anzahl der Zeiteinheiten, die das Tier hungern musste


	unsigned int contains_plant : 1; // speicher, ob auf diesem Feld auch pflanzen existieren

};

struct Map* init_map();

void init_population(struct Map *map, struct Segment *segment);

void printToBitmap(struct Map *map, int step);

struct Field* get_field(struct Map *map, int x, int y);

void copyFieldToOtherField(struct Field *sourceField, struct Field *targetField);

void moveFieldToOtherField(struct Field **sourceField, struct Field *targetField);

void reset_field(struct Field *field);

#endif
