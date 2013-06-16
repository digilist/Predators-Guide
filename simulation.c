#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "simulation.h"
#include "map.h"

/**
 * Richtungen in die sich ein Tier bewegen kann
 *
 */
enum Direction
{
	UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT, NUMBER_OF_DIRECTIONS
};

struct Coordinates
{
	int x;
	int y;
};

struct Coordinates* getRandomMovementOrder(struct Map *map);

int checkForPrey(struct Map *map, struct Field **field);

int moveAnimal(struct Map *map, struct Field **field);

void createChild(struct Map *map, struct Field *field);

struct Field* getRandomEmptyNeighboringField(struct Map *map, struct Field *field);

struct Field* getNeighboringFieldInDirection(struct Map *map, int x, int y,
		enum Direction direction);

/**
 * Simulation eines einzelnen Schrites auf dem Spielfeld
 *
 */
void simulationStep(struct Map *map, int step)
{
	struct Coordinates *movements;
	movements = getRandomMovementOrder(map);
	for (int i = 0; i < map->width * map->height; i++)
	{
		struct Field *field = getField(map, movements[i].x, movements[i].y);

		if (field->populationType == PLANT)
		{
			if (field->age >= MATURITY_AGE[field->populationType])
			{
				resetField(field);
			}

			if (field->populationType != EMPTY) // wenn nicht gestorben
				field->age++;
		}
		else if (field->populationType != EMPTY && field->lastStep < step)// 2. Bedinung zur Absicherung, damit
																		  // kein Tier in einer Runde mehrfach
																		  // Aktionen ausführt (z.B. durch Bewegung)
		{
			field->lastStep++;
			int moved = 0;

			// suche nach Beute
			moved = checkForPrey(map, &field); // moved ^= Beute gefunden

			// Bewegung
			if (!moved)
			{
				moved = moveAnimal(map, &field);
			}

			// überprüfe Alter auf Schwangerschaft
			if (field->age >= MATURITY_AGE[field->populationType])
			{
				createChild(map, field);
			}

			// wenn das Tier zu alt ist, stirbt es
			if (field->age > ELDERLY_AGE[field->populationType])
			{
				resetField(field);
			}
			else
			{
				// Tiere ohne Beute verhungern irgendwann
				if (field->populationType == HERBIVORE || field->populationType == CARNIVORE)
				{
					if (field->starveTime > MAX_STARVE_TIME[field->populationType])
					{
						resetField(field);
					}
					else
					{
						field->starveTime++;
					}
				}
			}

			if (field->populationType != EMPTY) // wenn nicht gestorben
				field->age++;
		}

		if (field->populationType == EMPTY)
		{
//				if(rand() % (3 * MAX_PLANT_AGE) == 0)
			if (rand() % ELDERLY_AGE[PLANT] == 0)
			{
				field->populationType = PLANT;
			}
		}
	}
}

struct Coordinates* getRandomMovementOrder(struct Map *map)
{
	struct Coordinates *movements = malloc(sizeof(struct Coordinates) * map->width * map->height);

	int i = 0;
	for (int x = 0; x < map->width; x++)
	{
		for (int y = 0; y < map->height; y++)
		{
			movements[i].x = x;
			movements[i].y = y;

			i++;
		}
	}

	// shuffle
	int n = map->width * map->height;
	for (i = n - 1; i >= 0; i--)
	{
		int j = rand() % n;
		struct Coordinates tmp = movements[j];
		movements[j] = movements[i];
		movements[i] = tmp;
	}

	return movements;
}

/**
 * Suche nach Beute auf Nachbarfeldern und fresse sie falls vorhanden,
 * dabei geht er auf das Feld der Beute und hat seine Bewegung beendet
 *
 * wurde Beute gefunden und gefressen, wird 1 zurückgegeben andernfalls 0
 */
int checkForPrey(struct Map *map, struct Field **field)
{
	for (int i = 0; i < NUMBER_OF_DIRECTIONS; i++) // alle umliegenden Felder prüfen
	{
		struct Field *neighboringField = getNeighboringFieldInDirection(map, (*field)->x,
				(*field)->y, i);

		if (((*field)->populationType == CARNIVORE && neighboringField->populationType == HERBIVORE)
				|| // Fleischfresser frisst Pflanzenfresser
				((*field)->populationType == HERBIVORE && neighboringField->populationType == PLANT)) // Pflanzenfresser frisst Pflanze
		{
			moveFieldToOtherField(field, neighboringField);
			(*field)->starveTime = 0;

			return 1;
		}
	}

	return 0;
}
/**
 * Tier zu einem zufälligen Nachbarfeld bewegen
 *
 * hat sich das Tier bewegt wird 1, andernfalls 0
 */
int moveAnimal(struct Map *map, struct Field **field)
{
	struct Field *neighboringField = getRandomEmptyNeighboringField(map, *field);

	if (neighboringField)
	{
		moveFieldToOtherField(field, neighboringField);
		return 1;
	}

	return 0;
}

/**
 * Ein Kind derselben Klasse auf einem zufälligen Nachbarfeld gebähren
 */
void createChild(struct Map *map, struct Field *field)
{
	struct Field *neighboringField = getRandomEmptyNeighboringField(map, field);
	if (neighboringField)
	{
		neighboringField->populationType = field->populationType;
		return;
	}

	if(field->populationType == HERBIVORE)
	{
		// Pflanzenfresser können auch auf Pflanzen Kinder zur Welt bringen, weil Sie dann direkt gefressen werden
		// Suche nächstes Feld mit Pflanze

		for(int i = 0; i < NUMBER_OF_DIRECTIONS; i++)
		{
			neighboringField = getNeighboringFieldInDirection(map, field->x, field->y, i);
			if(neighboringField->populationType == PLANT)
			{
				resetField(neighboringField);
				neighboringField->populationType = field->populationType;
			}
		}
	}
}

/**
 * gibt ein zufälliges benachbartes Feld zurück
 *
 */
struct Field* getRandomNeighboringField(struct Map *map, struct Field *field)
{
	enum Direction direction = rand() % 4;
	struct Field *neighboringField = getNeighboringFieldInDirection(map, field->x, field->y,
			direction);

	return neighboringField;
}

/**
 * gibt ein zufälliges benachbartes Feld zurück, das noch frei ist
 * ist keines mehr frei, wird 0 zurückgegeben
 *
 */
struct Field* getRandomEmptyNeighboringField(struct Map *map, struct Field *field)
{
	enum Direction direction = rand() % NUMBER_OF_DIRECTIONS;
	for (int i = 0; i < 4; i++)
	{
		struct Field *neighboringField = getNeighboringFieldInDirection(map, field->x, field->y,
				direction);
		if (neighboringField->populationType == EMPTY)
		{
			return neighboringField;
		}

		i++;
		direction = (direction + 1) % 4;
	}

	return 0;
}

/**
 * gibt das benachbarte Feld in der gegebenen Richtung zurück
 */
struct Field* getNeighboringFieldInDirection(struct Map *map, int x, int y,
		enum Direction direction)
{

	if (direction == UP || direction == UP_LEFT || direction == UP_RIGHT)
	{
		y += 1;
	}
	if (direction == DOWN || direction == DOWN_LEFT || direction == DOWN_RIGHT)
	{
		y -= 1;
	}
	if (direction == RIGHT || direction == UP_RIGHT || direction == DOWN_RIGHT)
	{
		x += 1;
	}
	if (direction == LEFT || direction == UP_LEFT || direction == DOWN_LEFT)
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
