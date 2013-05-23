#include <stdio.h>
#include <stdlib.h>
#include "field.h"

struct PlayingField {
	int width;
	int height;
	enum FieldPopulation *population;
};

enum FieldPopulation {
	EMPTY,
	PREDATOR,
	PREY
};

/**
 * 	Initialisiert das Spielfeld mit der angebenen Breite und Höhe
 * 
 */
void initField(int width, int height)
{
	struct PlayingField *field = malloc(sizeof(struct PlayingField));
	field->width = width;
	field->height = height;
	field->population = malloc(sizeof(enum FieldPopulation));
	
	printf("Initializing a %dx%d field\n", field->width, field->height);
}

/**
 * Simulation eines einzelnen Schrites auf dem Spielfeld
 *
 */
void simulationStep(struct PlayingField field)
{

}
