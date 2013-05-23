#include <stdio.h>
#include <stdlib.h>
#include "field.h"

/**
 * 	Initialisiert das Spielfeld mit der angebenen Breite und Höhe
 *
 */
void initField(int width, int height) {
	struct PlayingField *field = malloc(sizeof(struct PlayingField));
	field->width = width;
	field->height = height;
	field->numberOfPredators = 0;
	field->numberOfPrey = 0;
	field->population = malloc(sizeof(struct Field) * width * height);

	for (int i = 0; i < field->width; i++) {
		for (int j = 0; j < field->height; j++) {

			struct Field *cont = getField(field, i, j);

			(cont)->populationType = EMPTY;
			(cont)->age = 4;
			(cont)->starveTime = 0;
		}
	}

	printf("Initializing a %dx%d field\n", field->width, field->height);

	dumpPopulation(field);
}

void dumpPopulation(struct PlayingField *field) {
	for (int i = 0; i < field->width; i++) {
		for (int j = 0; j < field->height; j++) {
			struct Field *cont = getField(field, i, j);
			printf("%d\n", (cont)->age);
		}
	}
}

/**
 * Simulation eines einzelnen Schrites auf dem Spielfeld
 *
 */
void simulationStep(struct PlayingField *field) {

}

void* getField(struct PlayingField *field, int x, int y) {
	return field->population + (x * field->width + y);
}
