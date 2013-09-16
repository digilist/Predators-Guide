#include "config.h"

#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

/**
 * Repräsentiert das Spielfeld
 *
 */
struct Map
{
	int width;
	int height;

	struct Field *fields; // Belegung der einzelnen Felder des Spielfelds
	int numberOfPredators; // Anzahl Räuber aktuell
	int numberOfPrey; // Anzahl Beute aktuell
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
	int x, y; // Position des Feldes

	enum PopulationType populationType;

	// wenn populationType == EMPTY muss keines dieser Werte gesetzt werden
	int age; // erreicht das Lebewesen ein bestimmtes Alter, bringt es ein neues Lebewesen auf die Welt

	int starveTime; // Anzahl der Zeiteinheiten, die der Räuber hungern muss (nur für Räuber)
					// nach x ZE stirbt der Räuber

	int lastStep; // speichert, wann das Lebewesen seinen letzten Schritt vollzogen hat (um doppelte Schritte nach Bewegungen zu vermeiden)

	int containsPlant; // speicher, ob auf diesem Feld auch pflanzen existieren

};

struct Map* initMap();

void printToBitmap(struct Map *map, int step);

struct Field* getField(struct Map *map, int x, int y);

void copyFieldToOtherField(struct Field *sourceField, struct Field *targetField);

void moveFieldToOtherField(struct Field **sourceField, struct Field *targetField);

void resetField(struct Field *field);

#endif
