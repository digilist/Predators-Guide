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
	for (int i = 0; i < map->width; i++) {
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
void moveAnimal(struct Map *map, int x, int y) {
}

/**
 * Ein Kind derselben Klasse auf einem zufälligen Nachbarfeld gebähren
 */
void createChild(struct Map *map, int x, int y) {
	struct Field *field = getField(map, x, y);

	int i = 0;
	enum Direction direction = rand() % 4;
	while(i < 4) // test for all possible directions
	{
		struct Field *neighboredField = getNeighboringFieldInDirection(map, x, y, direction);
		if(neighboredField->populationType == EMPTY)
		{
			neighboredField->populationType = field->populationType;
			break;
		}
		else
		{
			i++;
			direction = (direction + 1) % 4;
		}
	}
}

/**
 * Suche nach Beute auf Nachbarfeldern und fresse sie falls vorhanden
 */
int checkForPrey(struct Map *map, int i, int j) {
	return 0;
}

struct Field* getNeighboringFieldInDirection(struct Map *map, int x, int y,
		enum Direction direction) {

	if (direction == UP) {
		y += 1;
	} else if (direction == DOWN) {
		y -= 1;
	} else if (direction == RIGHT) {
		x += 1;
	} else if (direction == LEFT) {
		x -= 1;
	}

	 // only positive modulo
	x += map->width;
	y += map->height;

	x %= map->width;
	y %= map->height;

	return getField(map, x, y);
}
