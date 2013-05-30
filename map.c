#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#include "simulation.h"
#include "bmp.h"

/**
 * 	Initialisiert das Spielfeld mit der angebenen Breite und Höhe
 *
 */
struct Map* initMap(int width, int height) {
	printf("Initializing a %dx%d field\n", width, height);

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

			int pop = rand() % 10;
			if(pop > 2)
				pop = 0; // >80% empty

			field->populationType = pop; // RANDOM population Type
		}
	}

	return map;
}

void printToBitmap(struct Map *map) {
	int width = map->width;
	int height = map->height;

	char *pixelMap;
	pixelMap = malloc(sizeof(char) * width * height * 3);

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			struct Field *field = getField(map, i, j);

			int index = 3 * (j * width + i);
			pixelMap[index] = 0; // r
			pixelMap[index + 1] = 0; // g
			pixelMap[index + 2] = 0; // b

			if(field->populationType == PREDATOR)
				pixelMap[index] = 255;
			else if(field->populationType == PREY)
				pixelMap[index + 1] = 255;
		}
		write_bmp("/tmp/test.bmp", width, height, pixelMap);
	}

}

struct Field* getField(struct Map *map, int x, int y) {
	return map->fields + (y * map->width + x);
}

void resetField(struct Field *field) {
	field->populationType = EMPTY;
	field->age = 0;
	field->starveTime = 0;
}

