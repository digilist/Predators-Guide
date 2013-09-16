#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "config.h"
#include "map.h"
#include "simulation.h"
#include "bmp.h"

char abstractBitmapFilepath[256];

/**
 * 	Initialisiert das Spielfeld mit der angebenen Breite und Höhe
 *
 */
struct Map* initMap()
{
	sprintf(abstractBitmapFilepath, "%s%s", SAVE_PATH, BITMAP_FILENAME);

	struct Map *map = malloc(sizeof(struct Map));
	map->width = MAP_WIDTH;
	map->height = MAP_HEIGHT;
	map->fields = malloc(sizeof(struct Field) * map->width * map->height);

	printf("Initializing a %dx%d field\n", map->width, map->height);

	int counter[NUMBER_OF_POPULATION_TYPES];
	for (int i = 0; i < NUMBER_OF_POPULATION_TYPES; i++)
		counter[i] = 0;

	int fillRate = MAP_FILL_RATE * 100;
	int predatorRate = PREDATOR_RATE * 100;
	int preyRate = PREY_RATE * 100;
	int plantRate = PLANT_RATE * 100;

	for (int i = 0; i < map->width; i++)
	{
		for (int j = 0; j < map->height; j++)
		{
			struct Field *field = getField(map, i, j);
			resetField(field);

			int pop = EMPTY;
			if(randomInt(0, 100) <= fillRate)
			{
				if(randomInt(0, 100) <= predatorRate)
				{
					pop = PREDATOR;
				}
				else if(randomInt(0, 100) <= preyRate)
				{
					pop = PREY;
				}
			}

			if(randomInt(0, 100) <= plantRate)
			{
				field->containsPlant= 1;
			}

			counter[pop]++;

			if(pop != EMPTY)
				field->age = rand() % ELDERLY_AGE[pop]; // RANDOM age

			field->populationType = pop; // RANDOM population Type
			field->x = i;
			field->y = j;
		}
	}

	printf("Spawned %d Prey\n", counter[PREY]);
	printf("        %d Predators\n", counter[PREDATOR]);
	printf("%d fields left Empty\n", counter[EMPTY]);

	return map;
}

/**
 * gibt das Spielfeld in eine Bitmap mit dem angebenen Dateipfad aus
 *
 */
void printToBitmap(struct Map *map, int step)
{
	if(!PRINTING_ENABLED)
		return;

	char filepath[256];
	sprintf(filepath, abstractBitmapFilepath, step);

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
				pixelMap[index] = 255; // predetaors are red
			else if (field->populationType == PREY)
				pixelMap[index + 2] = 255; // preys are blue
//			else if (field->containsPlant)
//				pixelMap[index + 1] = 255;
			else if(field->populationType == EMPTY)
			{
				pixelMap[index] = 255;
				pixelMap[index + 1] = 255;
				pixelMap[index + 2] = 255;
			}
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
void copyFieldToOtherField(struct Field *sourceField, struct Field *targetField)
{
	targetField->populationType = sourceField->populationType;
	targetField->age = sourceField->age;
	targetField->energy = sourceField->energy;
	targetField->lastStep = sourceField->lastStep;
}

/**
 * verschiebt ein Feld auf das Zielfeld
 * und schreibt die neue Adresse des Zielfeldes auf das Sourcefeld
 */
void moveFieldToOtherField(struct Field **sourceField, struct Field *targetField)
{
	copyFieldToOtherField(*sourceField, targetField);
	resetField(*sourceField);

	*sourceField = targetField;
}

/**
 * setzt ein Feld zurück, so dass dort kein Lebewesen mehr existiert
 *
 */
void resetField(struct Field *field)
{
	field->populationType = EMPTY;
	field->age = 0;
	field->energy = 0;
	field->lastStep = 0;
}

