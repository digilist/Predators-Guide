#include <stdio.h>
#include <stdlib.h>
#include "simulation.h"
#include "map.h"

const int MATURITY_AGE = 8;
const int ELDERLY_AGE = 12;
const int MAX_STARVE_TIME = 4;

/**
 * Simulation eines einzelnen Schrites auf dem Spielfeld
 *
 */
void simulationStep(struct Map *map) {
	for(int i = 0; i< map->width; i++) {
		for (int j = 0; j < map->height; j++) {
			struct Field *field = getField(map, i, j);

			if (field->populationType != EMPTY) {

				int moved = 0;

				// suche nach Beute
				if (field->populationType == PREDATOR) {
					moved = checkForPrey(map, i, j);
				}

				// Bewegung
				if (!moved) {
					moveAnimal(map, i, j);
				}

				// überprüfe Alter auf Schwangerschaft
				if (field->age > MATURITY_AGE) {
					// generate child
					createChild(map, i, j);
				}

				field->age++;

				// wenn das Tier zu alt ist, stirbt es
				if (field->age > ELDERLY_AGE) {
					resetField(field);
				}

				// Jäger ohne Beute verhungern irgendwann
				if (field->populationType == PREDATOR) {
					if (field->starveTime > MAX_STARVE_TIME) {
						resetField(field);
					} else {
						field->starveTime++;
					}
				}

			}
		}
	}
}

/**
 * Tier zu einem zufälligen Nachbarfeld bewegen
 */
void moveAnimal(struct Map *map, int i, int j) {}

/**
 * Ein Kind derselben Klasse auf einem zufälligen Nachbarfeld gebähren
 */
void createChild(struct Map *map, int i, int j) {}

/**
 * Suche nach Beute auf Nachbarfeldern und fresse sie falls vorhanden
 */
int checkForPrey(struct Map *map, int i, int j) { return 0; }

