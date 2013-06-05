#include <stdio.h>
#include <stdlib.h>
#include "simulation.h"
#include "map.h"

const int MATURITY_AGE = 8; // Gebähralter
const int ELDERLY_AGE = 12; // Sterbelter
const int MAX_STARVE_TIME = 4; // max. Jahre Hunger

/**
 * Simulation eines einzelnen Schrites auf dem Spielfeld
 *
 */
void simulationStep(struct Map *map)
{
	for (int i = 0; i < map->width; i++)
	{
		for (int j = 0; j < map->height; j++)
		{
			struct Field *field = getField(map, i, j);

			if (field->populationType != EMPTY)
			{
				int moved = 0;

				// suche nach Beute
				if (field->populationType == PREDATOR)
				{
					moved = checkForPrey(map, i, j);
				}

				// Bewegung
				if (!moved)
				{
					struct Field *targetField = moveAnimal(map, i, j);
					if (targetField)
					{
						field = targetField;
						moved = 1;
					}
				}

				// überprüfe Alter auf Schwangerschaft
				if (field->age > MATURITY_AGE)
				{
					// generate child
					createChild(map, i, j);
				}

				field->age++;

				// wenn das Tier zu alt ist, stirbt es
				if (field->age > ELDERLY_AGE)
				{
					resetField(field);
				}

				// Jäger ohne Beute verhungern irgendwann
				if (field->populationType == PREDATOR)
				{
					if (field->starveTime > MAX_STARVE_TIME)
					{
						resetField(field);
					}
					else
					{
						field->starveTime++;
					}
				}

			}
		}
	}
}

/**
 * Tier zu einem zufälligen Nachbarfeld bewegen
 *
 * hat sich das Tier bewegt wird das neue Feld zurückgegeben, andernfalls 0
 */
struct Field* moveAnimal(struct Map *map, int x, int y)
{
	struct Field *field = getField(map, x, y);
	struct Field *neighboredField = getRandomEmptyNeighboredField(map, x, y);

	if (neighboredField)
	{
		moveFieldToOtherField(field, neighboredField);
		return neighboredField;
	}

	return 0;
}

/**
 * Ein Kind derselben Klasse auf einem zufälligen Nachbarfeld gebähren
 */
void createChild(struct Map *map, int x, int y)
{
	struct Field *field = getField(map, x, y);

	struct Field *neighboredField = getRandomEmptyNeighboredField(map, x, y);
	if (neighboredField)
	{
		neighboredField->populationType = field->populationType;
		neighboredField->populationType = PREDATOR;
	}
}

/**
 * gibt ein zufälliges benachbartes Feld zurück, das noch frei ist
 * ist keines mehr frei, wird 0 zurückgegeben
 *
 */
struct Field* getRandomEmptyNeighboredField(struct Map *map, int x, int y)
{
	struct Field *field = getField(map, x, y);

	enum Direction direction = rand() % 4;
	for (int i = 0; i < 4; i++)
	{
		struct Field *neighboredField = getNeighboringFieldInDirection(map, x, y, direction);
		if (neighboredField->populationType == EMPTY)
		{
			return neighboredField;
		}

		i++;
		direction = (direction + 1) % 4;
	}

	return 0;
}

/**
 * Suche nach Beute auf Nachbarfeldern und fresse sie falls vorhanden
 */
int checkForPrey(struct Map *map, int i, int j)
{
	return 0;
}

struct Field* getNeighboringFieldInDirection(struct Map *map, int x, int y,
		enum Direction direction)
{

	if (direction == UP)
	{
		y += 1;
	}
	else if (direction == DOWN)
	{
		y -= 1;
	}
	else if (direction == RIGHT)
	{
		x += 1;
	}
	else if (direction == LEFT)
	{
		x -= 1;
	}

	// only positive modulo
	x += map->width;
	y += map->height;

	x %= map->width;
	y %= map->height;

	return getField(map, x, y);
}
