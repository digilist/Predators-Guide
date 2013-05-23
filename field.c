#include <stdio.h>
#include <stdlib.h>
#include "field.h"

/**
 * 	Initialisiert das Spielfeld mit der angebenen Breite und Höhe
 *
 */
void initField(int width, int height) {
	struct PlayingField *field = malloc(sizeof(struct PlayingField) * 1000);
	field->width = width;
	field->height = height;
	field->numberOfPredators = 0;
	field->numberOfPreys = 0;
	field->population = malloc(sizeof(struct FieldContent*) * width * height);

	for (int i = 0; i < field->width; i++) {
		for (int j = 0; j < field->height; j++) {

			struct FieldContent *cont = getFieldContent(field, i, j);
			cont->age = 0;
		}
	}

	printf("Initializing a %dx%d field\n", field->width, field->height);

	dumpPopulation(field);
}

void dumpPopulation(struct PlayingField *field) {
//	printf("%d", field->population[0][0]->age);
//	for (int i = 0; i < field->width; i++) {
//		for (int j = 0; j < field->height; j++) {
//			printf("%d", field->population[i][j]->age);
//		}
//	}
}

/**
 * Simulation eines einzelnen Schrites auf dem Spielfeld
 *
 */
void simulationStep(struct PlayingField *field) {

}

void* getFieldContent(struct PlayingField *field, int x, int y) {
	return &(*((*(field->population + x)) + y));
}
