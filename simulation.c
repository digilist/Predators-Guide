#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "simulation.h"
#include "map.h"

/**
 * Simulation eines einzelnen Schrites auf dem Spielfeld
 *
 */
void simulationStep(struct Map *map, int step)
{
	for (int i = 0; i < map->width; i++)
	{
		for (int j = 0; j < map->height; j++)
		{
			struct Field *field = getField(map, i, j);

			if (field->populationType != EMPTY && field->lastStep < step)
			{
				field->lastStep++;
				int moved = 0;

				// suche nach Beute
				if (field->populationType == PREDATOR)
				{
					struct Field *targetField = checkForPrey(map, field);
					if(targetField)
					{
						field = targetField;
						moved = 1;
					}
				}

				// Bewegung
				if (!moved)
				{
					struct Field *targetField = moveAnimal(map, field);
					if (targetField)
					{
						field = targetField;
						moved = 1;
					}
				}

				// überprüfe Alter auf Schwangerschaft
				if (field->age >= MATURITY_AGE)
				{
					//if(field->populationType != PREDATOR || rand() % 2 == 0) // Jäger bringen nur mit 25% Wahrscheinlichkeit ein Kind zur Welt

					if(rand() % 3 == 0) // nurin 33% der Fälle ein Kind zur Welt bringen
						createChild(map, field);
				}

				// wenn das Tier zu alt ist, stirbt es
				if (field->age > ELDERLY_AGE)
				{
					resetField(field);
				}
				else
				{
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

				if(field->populationType != EMPTY) // if not reset
					field->age++;
			}
		}
	}
}

/**
 * Suche nach Beute auf Nachbarfeldern und fresse sie falls vorhanden,
 * dabei geht er auf das Feld der Beute und hat seine Bewegung beendet
 *
 * gibt die Adresse des neuen Feldes zurück, wenn es keine bedeute findet die 0
 */
struct Field* checkForPrey(struct Map *map, struct Field *field)
{
	for(int i = 0; i < 4; i++) // test all directions
	{
		struct Field *neighboredField = getNeighboringFieldInDirection(map, field->x, field->y, i); // possible prey
		if(neighboredField->populationType == PREY)
		{
			moveFieldToOtherField(field, neighboredField);
			resetField(field);

			// neighboredField now contains Predator
			neighboredField->starveTime = 0;

			return neighboredField;
		}
	}

	return 0;
}
/**
 * Tier zu einem zufälligen Nachbarfeld bewegen
 *
 * hat sich das Tier bewegt wird das neue Feld zurückgegeben, andernfalls 0
 */
struct Field* moveAnimal(struct Map *map, struct Field *field)
{
	struct Field *neighboredField = getRandomEmptyNeighboredField(map, field);

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
void createChild(struct Map *map, struct Field *field)
{
	struct Field *neighboredField = getRandomEmptyNeighboredField(map, field);
	if (neighboredField)
	{
		neighboredField->populationType = field->populationType;
	}
}

/**
 * gibt ein zufälliges benachbartes Feld zurück
 *
 */
struct Field* getRandomNeighboredField(struct Map *map, struct Field *field)
{
	enum Direction direction = rand() % 4;
	struct Field *neighboredField = getNeighboringFieldInDirection(map, field->x, field->y, direction);

	return neighboredField;
}

/**
 * gibt ein zufälliges benachbartes Feld zurück, das noch frei ist
 * ist keines mehr frei, wird 0 zurückgegeben
 *
 */
struct Field* getRandomEmptyNeighboredField(struct Map *map, struct Field *field)
{
	enum Direction direction = rand() % 4;
	for (int i = 0; i < 4; i++)
	{
		struct Field *neighboredField = getNeighboringFieldInDirection(map, field->x, field->y, direction);
		if (neighboredField->populationType == EMPTY)
		{
			return neighboredField;
		}

		i++;
		direction = (direction + 1) % 4;
	}

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
