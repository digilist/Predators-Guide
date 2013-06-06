#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "config.h"
#include "map.h"
#include "simulation.h"
#include "bmp.h"

/**
 * 	Initialisiert das Spielfeld mit der angebenen Breite und Höhe
 *
 */
struct Map* initMap(int width, int height)
{
	printf("Initializing a %dx%d field\n", width, height);

	struct Map *map = malloc(sizeof(struct Map));
	map->width = width;
	map->height = height;
	map->numberOfPredators = 0;
	map->numberOfPrey = 0;
	map->fields = malloc(sizeof(struct Field) * width * height);

	int predatorCounter = 0;
	int preyCounter = 0;
	int emptyCounter = 0;

	for (int i = 0; i < map->width; i++)
	{
		for (int j = 0; j < map->height; j++)
		{
			struct Field *field = getField(map, i, j);
			resetField(field);

			int pop = rand() % 100;
			if (pop < 10)
			{
				pop = PREDATOR;
				predatorCounter++;
			}
			else if (pop < 30)
			{
				pop = PREY;
				preyCounter++;
			}
			else
			{
				pop = EMPTY;
				emptyCounter++;
			}

			field->populationType = pop; // RANDOM population Type
			field->age = rand() % ELDERLY_AGE; // RANDOM age
			field->x = i;
			field->y = j;
		}
	}

	printf("Placed %d Predators, %d Prey (%d empty fields)\n", predatorCounter, preyCounter,
			emptyCounter);

	return map;
}

/**
 * gibt das Spielfeld in eine Bitmap mit dem angebenen Dateipfad aus
 *
 */
void printToBitmap(struct Map *map, char* filepath)
{
	int width = map->width;
	int height = map->height;

	char *pixelMap;
	pixelMap = malloc(sizeof(char) * width * height * 3);

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			struct Field *field = getField(map, i, j);

			int index = 3 * (j * width + i);
			pixelMap[index] = 0; // r
			pixelMap[index + 1] = 0; // g
			pixelMap[index + 2] = 0; // b

			if (field->populationType == PREDATOR)
				pixelMap[index] = 255; // predator is red
			else if (field->populationType == PREY)
				pixelMap[index + 1] = 255; // prey is green
		}
	}

	// scale image (remove in final version)
	int scaleFactor = SCALE_FACTOR;
	if(scaleFactor > 1 && width == height)
	{
		int scaledWidth = scaleFactor * width;
		int scaledHeight = scaleFactor * height;

		char *scaledPixelMap;
		scaledPixelMap = malloc(sizeof(char) * scaledWidth * scaledHeight * 3);

		int px, py;
		for(int i = 0; i < scaledWidth; i++)
		{
			for(int j = 0; j < scaledHeight; j++)
			{
				px = (int) (j * (1.0 / scaleFactor));
				py = (int) (i * (1.0 / scaleFactor));

				int index = 3 * (i * scaledWidth + j);
				int origIndex = 3 * (py * width + px);

				scaledPixelMap[index] = pixelMap[origIndex];
				scaledPixelMap[index + 1] = pixelMap[origIndex + 1];
				scaledPixelMap[index + 2] = pixelMap[origIndex + 2];
			}
		}

		write_bmp(filepath, scaledWidth, scaledHeight, scaledPixelMap);
	}
	else
	{
		write_bmp(filepath, width, height, pixelMap);
	}


}

/**
 * Gibt ein einzelnes Feld auf der Karte zurück
 *
 */
struct Field* getField(struct Map *map, int x, int y)
{
	return map->fields + (y * map->width + x);
}

/**
 * kopiert ein Feld auf ein anderes Feld
 *
 */
void moveFieldToOtherField(struct Field *sourceField, struct Field *targetField)
{
	targetField->populationType = sourceField->populationType;
	targetField->age = sourceField->age;
	targetField->starveTime = sourceField->starveTime;
	targetField->lastStep = sourceField->lastStep;

	resetField(sourceField);
}

/**
 * setzt ein Feld zurück, so dass dort kein Lebewesen mehr existiert
 *
 */
void resetField(struct Field *field)
{
	field->populationType = EMPTY;
	field->age = 0;
	field->starveTime = 0;
	field->lastStep = 0;
}

