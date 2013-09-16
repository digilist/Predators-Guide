#include "config.h"

#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

/**
 * Repräsentiert das Spielfeld
 *
 */
struct Map
{
	unsigned int width;
	unsigned int height;

	struct Field *fields; // Belegung der einzelnen Felder des Spielfelds
	unsigned int numberOfPredators; // Anzahl Räuber aktuell
	unsigned int numberOfPrey; // Anzahl Beute aktuell
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

	enum PopulationType populationType : 2;

	unsigned int lastStep : 16; // speichert, wann das Lebewesen seinen letzten Schritt vollzogen hat (um doppelte Schritte nach Bewegungen zu vermeiden)

	// wenn populationType == EMPTY muss keines dieser Werte gesetzt werden
	unsigned int age : 8; // erreicht das Lebewesen ein bestimmtes Alter, bringt es ein neues Lebewesen auf die Welt

	unsigned int energy : 8; // Anzahl der Zeiteinheiten, die das Tier hungern musste


	unsigned int containsPlant : 1; // speicher, ob auf diesem Feld auch pflanzen existieren

};

struct Map* initMap();

void printToBitmap(struct Map *map, int step);

struct Field* getField(struct Map *map, int x, int y);

void copyFieldToOtherField(struct Field *sourceField, struct Field *targetField);

void moveFieldToOtherField(struct Field **sourceField, struct Field *targetField);

void resetField(struct Field *field);

#endif
