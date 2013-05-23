#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#include "simulation.h"

/**
 * 	Initialisiert das Spielfeld mit der angebenen Breite und Höhe
 *
 */
void initField(int width, int height) {
	struct Map *map = malloc(sizeof(struct Map));
	map->width = width;
	map->height = height;
	map->numberOfPredators = 0;
	map->numberOfPrey = 0;
	map->fields = malloc(sizeof(struct Field) * width * height);

	for (int i = 0; i < map->width; i++) {
		for (int j = 0; j < map->height; j++) {

			struct Field *field = getField(map, i, j);

			resetField(field);
		}
	}

	printf("Initializing a %dx%d field\n", map->width, map->height);

	dumpPopulation(map);
}

void dumpPopulation(struct Map *map) {
	for (int i = 0; i < map->width; i++) {
		for (int j = 0; j < map->height; j++) {
			struct Field *field = getField(map, i, j);
			printf("%d\n", field->populationType);
		}
	}
}

void* getField(struct Map *map, int x, int y) {
	return map->fields + (y * map->width + x);
}

void resetField(struct Field *field) {
	field->populationType = EMPTY;
	field->age = 0;
	field->starveTime = 0;
}

