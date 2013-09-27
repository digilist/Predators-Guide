#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "simulation.h"
#include "map.h"
#include "result.h"
#include "parallel.h"

struct Coordinates
{
	int x;
	int y;
};

struct StepResult* calculate_step_result(struct Map *map, int step);

struct Coordinates* get_random_movement_order(struct Map *map);

int check_for_food(struct Map *map, struct Field **field);

int find_food(struct Map *map, struct Field *field, enum Direction previous_direction, int layer, enum PopulationType population_type);

int move_animal(struct Map *map, struct Field **field);

void create_child(struct Map *map, struct Field *field);

int should_get_child(int population_type);

int should_die(struct Field *field);

struct Field* get_random_empty_neighboring_field(struct Map *map, struct Field *field);

struct Field* get_neighboring_field_in_direction(struct Map *map, int x, int y, enum Direction direction);

/**
 * Simulation eines einzelnen Schrites auf dem Spielfeld
 *
 */
void simulation_step(struct Map *map, int step)
{
	struct Segment *segment = get_segment();

	struct Coordinates *movements;
	movements = get_random_movement_order(map);
	for (int i = 0; i < segment->width * segment->height; i++)
	{
		struct Field *field = get_field(map, movements[i].x, movements[i].y);

		if (field->population_type != EMPTY)
		{
			// suche nach Beute
			if(check_for_food(map, &field))
			{
				field->last_step++;
			}
		}
	}
	free(movements);

	movements = get_random_movement_order(map);
	for (int i = 0; i < segment->width * segment->height; i++)
	{
		struct Field *field = get_field(map, movements[i].x, movements[i].y);

		if (field->population_type != EMPTY)
		{
			struct Field *field = get_field(map, movements[i].x, movements[i].y);

			// hat sich dieses Tier diese Runde schon einmal bewegt? (vorher durchs Fressen oder durch eine Bewegung auf dieses Feld)
			if(field->last_step < step)
			{
				move_animal(map, &field);
			}

			// überprüfe Alter auf Schwangerschaft
			if (should_get_child(field->population_type))
			{
				create_child(map, field);
			}

			// wenn das Tier zu alt ist, stirbt es
			if (should_die(field))
			{
				reset_field(field);
			}
			else
			{
				// Tiere ohne Beute verhungern irgendwann
				if (field->energy <= 0)
				{
					reset_field(field);
				}
				else
				{
					field->energy--;
				}
			}

			if (field->population_type != EMPTY) // wenn nicht gestorben
				field->age++;
		}
		else if(field->population_type == EMPTY)
		{
			if(random_int(1, 100) <= (PLANT_RATE * 100))
			{
				field->contains_plant = 1;
			}
		}
	}
	free(movements);
}

/**
 * berechnet das Ergebnis der Runde
 * (wieviele Pflanzen, Beute und Jäger gibt es danach es)
 *
 */
struct StepResult* calculate_step_result(struct Map *map, int step)
{
	// statistics
	struct StepResult *resultset = malloc(sizeof(struct StepResult));
	resultset->amount_prey = 0;
	resultset->amount_predators = 0;
	resultset->current_step = step;
	resultset->next = 0;

	struct Segment *segment = get_segment();

	for (int x = segment->x1; x <= segment->x2; x++)
	{
		for (int y = segment->y1; y <= segment->y2; y++)
		{
			struct Field *field = get_field(map, x, y);

			if (field->population_type == PREY)
			{
				resultset->amount_prey++;
			}
			else if (field->population_type == PREDATOR)
			{
				resultset->amount_predators++;
			}
		}
	}

	return resultset;
}

/**
 * soll das Tier auf dem gegebenen Feld sterben?
 *
 */
int should_die(struct Field *field)
{
	int dying_rate = DYING_RATE[field->population_type] * 100;
	return field->age > ELDERLY_AGE[field->population_type] || (DYING_RATE[field->population_type] > 0 && random_int(1, 100) <= dying_rate);
}

/**
 * soll für die gegebene Rasse ein Kind geboren werden?
 *
 */
int should_get_child(int population_type)
{
	int birth_rate = BIRTH_RATE[population_type] * 100;
	return BIRTH_RATE[population_type] > 0 && random_int(1, 100) <= birth_rate;
}

/**
 * bringe alle Felder in eine zufällige Reihenfolge,
 * um ein zufälligeres Verhalten bei den Bewegungen zu erzeugen
 */
struct Coordinates* get_random_movement_order(struct Map *map)
{
	struct Segment *segment = get_segment();
	int fields = segment->width * segment->height;
	struct Coordinates *movements = malloc(sizeof(struct Coordinates) * fields);

	int i = 0;
	for (int x = segment->x1; x <= segment->x2; x++)
	{
		for (int y = segment->y1; y <= segment->y2; y++)
		{
			movements[i].x = x;
			movements[i].y = y;

			i++;
		}
	}

	// shuffle
	for (i = fields - 1; i >= 0; i--)
	{
		int j = rand() % fields;
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
int check_for_food(struct Map *map, struct Field **field)
{
	for (int i = 0; i < NUMBER_OF_DIRECTIONS; i++) // alle umliegenden Felder prüfen
	{
		struct Field *neighboring_field = get_neighboring_field_in_direction(map, (*field)->x, (*field)->y, i);

		if((*field)->population_type == PREDATOR)
		{
			if (neighboring_field->population_type == PREY) // Pflanzenfresser frisst Pflanze
			{
				move_field_to(field, neighboring_field);
				(*field)->energy = MAX_ENERGY;

				return 1;
			}
		}
		else if((*field)->population_type == PREY)
		{
			if (neighboring_field->contains_plant) // Pflanzenfresser frisst Pflanze
			{
				move_field_to(field, neighboring_field);
				(*field)->energy = MAX_ENERGY;
				(*field)->contains_plant = 0;

				return 1;
			}
		}
	}
	for(int i = 0; i < NUMBER_OF_DIRECTIONS; i++)
	{
		struct Field *neighboring_field = get_neighboring_field_in_direction(map, (*field)->x, (*field)->y, i);

		if(find_food(map, neighboring_field, i, 0, (*field)->population_type) == 1)
		{
			move_field_to(field, neighboring_field);

			return 1;
		}
	}

	return 0;
}

int find_food(struct Map *map, struct Field *field, enum Direction previous_direction, int layer, enum PopulationType population_type)
{
	struct Field *fieldList[17];
	enum Direction fieldListDirection[17];
	int count = 0;

	if (previous_direction < UP_LEFT)
	{
		struct Field *neighboringField = get_neighboring_field_in_direction(map, (field)->x, (field)->y, previous_direction);
		fieldList[count] = neighboringField;
		fieldListDirection[count] = previous_direction;
		count++;
	}
	else if (previous_direction == UP_LEFT)
	{
		struct Field *neighboringField1 = get_neighboring_field_in_direction(map, (field)->x, (field)->y, previous_direction);
		fieldList[count] = neighboringField1;
		fieldListDirection[count] = previous_direction;
		count++;

		struct Field *neighboringField2 = get_neighboring_field_in_direction(map, (field)->x, (field)->y, UP);
		fieldList[count] = neighboringField2;
		fieldListDirection[count] = UP;
		count++;

		struct Field *neighboringField3 = get_neighboring_field_in_direction(map, (field)->x, (field)->y, LEFT);
		fieldList[count] = neighboringField3;
		fieldListDirection[count] = LEFT;
		count++;
	}
	else if (previous_direction == UP_RIGHT)
	{
		struct Field *neighboringField1 = get_neighboring_field_in_direction(map, (field)->x, (field)->y, previous_direction);
		fieldList[count] = neighboringField1;
		fieldListDirection[count] = previous_direction;
		count++;

		struct Field *neighboringField2 = get_neighboring_field_in_direction(map, (field)->x, (field)->y, UP);
		fieldList[count] = neighboringField2;
		fieldListDirection[count] = UP;
		count++;

		struct Field *neighboringField3 = get_neighboring_field_in_direction(map, (field)->x, (field)->y, RIGHT);
		fieldList[count] = neighboringField3;
		fieldListDirection[count] = RIGHT;
		count++;
	}
	else if (previous_direction == DOWN_LEFT)
	{
		struct Field *neighboringField1 = get_neighboring_field_in_direction(map, (field)->x, (field)->y, previous_direction);
		fieldList[count] = neighboringField1;
		fieldListDirection[count] = previous_direction;
		count++;

		struct Field *neighboringField2 = get_neighboring_field_in_direction(map, (field)->x, (field)->y, DOWN);
		fieldList[count] = neighboringField2;
		fieldListDirection[count] = DOWN;
		count++;

		struct Field *neighboringField3 = get_neighboring_field_in_direction(map, (field)->x, (field)->y, LEFT);
		fieldList[count] = neighboringField3;
		fieldListDirection[count] = LEFT;
		count++;
	}
	else if (previous_direction == DOWN_RIGHT)
	{
		struct Field *neighboringField1 = get_neighboring_field_in_direction(map, (field)->x, (field)->y, previous_direction);
		fieldList[count] = neighboringField1;
		fieldListDirection[count] = previous_direction;
		count++;

		struct Field *neighboringField2 = get_neighboring_field_in_direction(map, (field)->x, (field)->y, DOWN);
		fieldList[count] = neighboringField2;
		fieldListDirection[count] = DOWN;
		count++;

		struct Field *neighboringField3 = get_neighboring_field_in_direction(map, (field)->x, (field)->y, RIGHT);
		fieldList[count] = neighboringField3;
		fieldListDirection[count] = RIGHT;
		count++;
	}
	for (int i = 0; i < count; i++)
	{
		if (fieldList[i]->population_type == PREY && population_type == PREDATOR)
		{
			return 1;
		}
		//else if(fieldList[i]->containsPlant && popuType == PREY)
		//{
		//	return 1;
		//}
		else if (layer == 0)
		{
			if (find_food(map, fieldList[i], fieldListDirection[i], 1, population_type) == 1)
			{
				return 1;
			}
		}
	}
	return 0;
}

/**
 * Tier zu einem zufälligen Nachbarfeld bewegen
 *
 * hat sich das Tier bewegt wird 1, andernfalls 0
 */
int move_animal(struct Map *map, struct Field **field)
{
	struct Field *neighboring_field = get_random_empty_neighboring_field(map, *field);

	if (neighboring_field)
	{
		move_field_to(field, neighboring_field);
		return 1;
	}

	return 0;
}

/**
 * Ein Kind derselben Klasse auf einem zufälligen Nachbarfeld gebähren
 */
void create_child(struct Map *map, struct Field *field)
{
	struct Field *neighboring_field = get_random_empty_neighboring_field(map, field);
	if (neighboring_field)
	{
		neighboring_field->population_type = field->population_type;
		neighboring_field->energy = MAX_ENERGY;

		send_field_if_border(neighboring_field);

		return;
	}
}

/**
 * gibt ein zufälliges benachbartes Feld zurück
 *
 */
struct Field* get_random_neighboring_field(struct Map *map, struct Field *field)
{
	enum Direction direction = rand() % NUMBER_OF_DIRECTIONS;
	struct Field *neighboring_field = get_neighboring_field_in_direction(map, field->x, field->y, direction);

	return neighboring_field;
}

/**
 * gibt ein zufälliges benachbartes Feld zurück, das noch frei ist
 * ist keines mehr frei, wird 0 zurückgegeben
 *
 */
struct Field* get_random_empty_neighboring_field(struct Map *map, struct Field *field)
{
	enum Direction direction = rand() % NUMBER_OF_DIRECTIONS;
	for (int i = 0; i < NUMBER_OF_DIRECTIONS; i++)
	{
		struct Field *neighboring_field = get_neighboring_field_in_direction(map, field->x, field->y, direction);
		if (neighboring_field->population_type == EMPTY)
		{
			return neighboring_field;
		}

		i++;
		direction = (direction + 1) % 4;
	}

	return 0;
}

/**
 * gibt das benachbarte Feld in der gegebenen Richtung zurück
 */
struct Field* get_neighboring_field_in_direction(struct Map *map, int x, int y, enum Direction direction)
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

	return get_field(map, x, y);
}

