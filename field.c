#include <stdio.h>
#include <stdlib.h>
#include "field.h"

/**
 * Repräsentiert das Spielfeld
 *
 */
struct PlayingField {
	int width;
	int height;

	struct FieldContent *population; // Belegung der einzelnen Felder des Spielfelds
	int numberOfPredators; // Anzahl Räuber aktuell
	int numberOfPreys; // Anzahl Beute aktuell
};

/**
 * die Belegung eines Feldes
 *
 */
struct FieldContent {
	enum FieldPopulationType *populationType;

	// wenn populationType == EMPTY muss keines dieser Werte gesetzt werden
	int age; // erreicht das Lebewesen ein bestimmtes Alter, bringt es ein neues Lebewesen auf die Welt

	int starveTime; // Anzahl der Zeiteinheiten, die der Räuber hungern muss (nur für Räuber)
					// nach x ZE stirbt der Räuber

};

/**
 * die möglichen Feldbelegungen: leeres Feld, Räuber & Beute
 *
 */
enum FieldPopulationType {
	EMPTY, PREDATOR, PREY
};

/**
 * Richtungen in die sich ein Tier bewegen kann
 *
 */
enum Direction {
	TOP, BOTTOM, LEFT, RIGHT
};

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
	field->population = malloc(sizeof(struct FieldContent) * width * height);

	printf("Initializing a %dx%d field\n", field->width, field->height);

	dumpPopulation(field);
}

void dumpPopulation(struct PlayingField *field) {
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
