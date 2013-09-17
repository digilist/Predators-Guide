#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "simulation.h"
#include "map.h"
#include "result.h"

/**
 * Richtungen in die sich ein Tier bewegen kann
 *
 * auf keinen Fall die Reihenfolge ändern!
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

struct StepResult* calculate_step_result(struct Map *map, struct Segment *segment, int step);

struct Coordinates* get_random_movement_order(struct Map *map, struct Segment *segment);

int checkForFood(struct Map *map, struct Field **field);

int findFood(struct Map *map, struct Field *field, enum Direction previousDirection, int layer, enum PopulationType popuType);

int moveAnimal(struct Map *map, struct Field **field);

void createChild(struct Map *map, struct Field *field);

int shouldGetChild(int populationType);

int shouldDie(struct Field *field);

struct Field* getRandomEmptyNeighboringField(struct Map *map, struct Field *field);

struct Field* getNeighboringFieldInDirection(struct Map *map, int x, int y, enum Direction direction);

/**
 * führt eine einzelne Simulation mit der gegebenen Konfiguration aus
 *
 */
//struct SimulationResult* run_simulation()
//{
//	struct Map *map = initMap();
//
//	int i = 0;
//	struct StepResult *current;
//	struct StepResult *first;
//	struct StepResult *temp_step_result;
//
////	int predator_trend;
////	int prey_trend;
////	int previous_predator_amount;
////	int previous_prey_amount;
//
//	// init first result
//	first = calculateStepResultset(map, 0);
//	current = first;
//	temp_step_result = first;
//
////	previous_predator_amount = first->amount_predators;
////	previous_prey_amount = first->amount_prey;
//
//	while (1)
//	{
//		printToBitmap(map, i);
//		i++;
//		printf("Simulation Step %d", i);
//
//		temp_step_result = simulation_step(map, i);
//
//		if (i > 2)
//		{ // there can be no turning point in the first two rounds
//
//			// check on turning point
////			if (((((previous_predator_amount - temp_step_result->amount_predators) < 0) != predator_trend)
////					&& (previous_predator_amount - temp_step_result->amount_predators) != 0)
////					|| ((((previous_prey_amount - temp_step_result->amount_prey) < 0) != prey_trend)
////							&& (previous_prey_amount - temp_step_result->amount_prey) != 0))
////			{
//
//				// attach result set to linked list
//				current->next = temp_step_result;
//				current = current->next;
//
////				printf("\nTURNING POINT %d/%d\n\n", previous_predator_amount, previous_prey_amount);
////			}
//		}
//
////		predator_trend = (previous_predator_amount - temp_step_result->amount_predators) < 0;
////		prey_trend = (previous_prey_amount - temp_step_result->amount_prey) < 0;
////		previous_predator_amount = temp_step_result->amount_predators;
////		previous_prey_amount = temp_step_result->amount_prey;
//
//		if (temp_step_result->amount_predators == 0 || temp_step_result->amount_prey == 0)
//		{
//			printf("\nOne species died!\n");
//			break;
//		}
//		printf(" - %d/%d\n", temp_step_result->amount_predators, temp_step_result->amount_prey);
//
//		if (i > MAX_SIMULATION_STEPS)
//			break;
//	}
//
//	struct SimulationResult *result = malloc(sizeof(struct SimulationResult));
//	result->simulationSteps = i;
//	result->firstStepResult = first;
//
//	return result;
//}
//
//
//struct SimulationResult* run_simulation()
//{
//	struct Map *map = init_map();
//
//	int i = 0;
//	struct StepResult *current;
//	struct StepResult *first;
//	struct StepResult *temp_step_result;
//
//	// init first result
//	first = calculateStepResultset(map, 0);
//	current = first;
//	temp_step_result = first;
//
//	while (1)
//	{
//		printToBitmap(map, i);
//		i++;
//
//		printf("Simulation Step %d", i);
//
//		temp_step_result = simulation_step(map, i);
//
//		current->next = temp_step_result;
//		current = current->next;
//
//		if (temp_step_result->amount_predators == 0 || temp_step_result->amount_prey == 0)
//		{
//			printf("\nOne species died!\n");
//			break;
//		}
//		printf(" - %d/%d\n", temp_step_result->amount_predators, temp_step_result->amount_prey);
//
//		if (i > MAX_SIMULATION_STEPS)
//			break;
//	}
//
//	struct SimulationResult *result = malloc(sizeof(struct SimulationResult));
//	result->simulationSteps = i;
//	result->firstStepResult = first;
//
//	return result;
//}

/**
 * Simulation eines einzelnen Schrites auf dem Spielfeld
 *
 */
void simulation_step(struct Map *map, struct Segment *segment, int step)
{
	struct Coordinates *movements;
	movements = get_random_movement_order(map, segment);
	for (int i = 0; i < segment->width * segment->height; i++)
	{
		struct Field *field = get_field(map, movements[i].x, movements[i].y);

		if (field->population_type != EMPTY)
		{
			// suche nach Beute
			if(checkForFood(map, &field))
			{
				field->lastStep++;
			}
		}
	}
	free(movements);

	movements = get_random_movement_order(map, segment);
	for (int i = 0; i < segment->width * segment->height; i++)
	{
		struct Field *field = get_field(map, movements[i].x, movements[i].y);

		if (field->population_type != EMPTY)
		{
			struct Field *field = get_field(map, movements[i].x, movements[i].y);

			// hat sich dieses Tier diese Runde schon einmal bewegt? (vorher durchs Fressen oder durch eine Bewegung auf dieses Feld)
			if(field->lastStep < step)
			{
				moveAnimal(map, &field);
			}

			// überprüfe Alter auf Schwangerschaft
			if (shouldGetChild(field->population_type))
			{
				createChild(map, field);
			}

			// wenn das Tier zu alt ist, stirbt es
			if (shouldDie(field))
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
struct StepResult* calculate_step_result(struct Map *map, struct Segment *segment, int step)
{
	// statistics
	struct StepResult *resultset = malloc(sizeof(struct StepResult));
	resultset->amount_prey = 0;
	resultset->amount_predators = 0;
	resultset->current_step = step;
	resultset->next = 0;

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
int shouldDie(struct Field *field)
{
	int dyingRate = DYING_RATE[field->population_type] * 100;
	return field->age > ELDERLY_AGE[field->population_type] || (DYING_RATE[field->population_type] > 0 && random_int(1, 100) <= dyingRate);
}

/**
 * soll für die gegebene Rasse ein Kind geboren werden?
 *
 */
int shouldGetChild(int populationType)
{
	int birthRate = BIRTH_RATE[populationType] * 100;
	return BIRTH_RATE[populationType] > 0 && random_int(1, 100) <= birthRate;
}

/**
 * bringe alle Felder in eine zufällige Reihenfolge,
 * um ein zufälligeres Verhalten bei den Bewegungen zu erzeugen
 */
struct Coordinates* get_random_movement_order(struct Map *map, struct Segment *segment)
{
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
int checkForFood(struct Map *map, struct Field **field)
{
	for (int i = 0; i < NUMBER_OF_DIRECTIONS; i++) // alle umliegenden Felder prüfen
	{
		struct Field *neighboringField = getNeighboringFieldInDirection(map, (*field)->x, (*field)->y, i);

		if((*field)->population_type == PREDATOR)
		{
			if (neighboringField->population_type == PREY) // Pflanzenfresser frisst Pflanze
			{
				moveFieldToOtherField(field, neighboringField);
				(*field)->energy = MAX_ENERGY;

				return 1;
			}
		}
		else if((*field)->population_type == PREY)
		{
			if (neighboringField->contains_plant) // Pflanzenfresser frisst Pflanze
			{
				moveFieldToOtherField(field, neighboringField);
				(*field)->energy = MAX_ENERGY;
				(*field)->contains_plant = 0;

				return 1;
			}
		}
	}
	for(int i = 0; i < NUMBER_OF_DIRECTIONS; i++)
	{
		struct Field *neighboringField = getNeighboringFieldInDirection(map, (*field)->x, (*field)->y, i);

		if(findFood(map, neighboringField, i, 0, (*field)->population_type) == 1)
		{
			moveFieldToOtherField(field, neighboringField);

			return 1;
		}
	}

	return 0;
}

int findFood(struct Map *map, struct Field *field, enum Direction previousDirection, int layer, enum PopulationType popuType)
{
	struct Field *fieldList[17];
	enum Direction fieldListDirection[17];
	int count = 0;

	if (previousDirection < UP_LEFT)
	{
		struct Field *neighboringField = getNeighboringFieldInDirection(map, (field)->x, (field)->y, previousDirection);
		fieldList[count] = neighboringField;
		fieldListDirection[count] = previousDirection;
		count++;
	}
	else if (previousDirection == UP_LEFT)
	{
		struct Field *neighboringField1 = getNeighboringFieldInDirection(map, (field)->x, (field)->y, previousDirection);
		fieldList[count] = neighboringField1;
		fieldListDirection[count] = previousDirection;
		count++;

		struct Field *neighboringField2 = getNeighboringFieldInDirection(map, (field)->x, (field)->y, UP);
		fieldList[count] = neighboringField2;
		fieldListDirection[count] = UP;
		count++;

		struct Field *neighboringField3 = getNeighboringFieldInDirection(map, (field)->x, (field)->y, LEFT);
		fieldList[count] = neighboringField3;
		fieldListDirection[count] = LEFT;
		count++;
	}
	else if (previousDirection == UP_RIGHT)
	{
		struct Field *neighboringField1 = getNeighboringFieldInDirection(map, (field)->x, (field)->y, previousDirection);
		fieldList[count] = neighboringField1;
		fieldListDirection[count] = previousDirection;
		count++;

		struct Field *neighboringField2 = getNeighboringFieldInDirection(map, (field)->x, (field)->y, UP);
		fieldList[count] = neighboringField2;
		fieldListDirection[count] = UP;
		count++;

		struct Field *neighboringField3 = getNeighboringFieldInDirection(map, (field)->x, (field)->y, RIGHT);
		fieldList[count] = neighboringField3;
		fieldListDirection[count] = RIGHT;
		count++;
	}
	else if (previousDirection == DOWN_LEFT)
	{
		struct Field *neighboringField1 = getNeighboringFieldInDirection(map, (field)->x, (field)->y, previousDirection);
		fieldList[count] = neighboringField1;
		fieldListDirection[count] = previousDirection;
		count++;

		struct Field *neighboringField2 = getNeighboringFieldInDirection(map, (field)->x, (field)->y, DOWN);
		fieldList[count] = neighboringField2;
		fieldListDirection[count] = DOWN;
		count++;

		struct Field *neighboringField3 = getNeighboringFieldInDirection(map, (field)->x, (field)->y, LEFT);
		fieldList[count] = neighboringField3;
		fieldListDirection[count] = LEFT;
		count++;
	}
	else if (previousDirection == DOWN_RIGHT)
	{
		struct Field *neighboringField1 = getNeighboringFieldInDirection(map, (field)->x, (field)->y, previousDirection);
		fieldList[count] = neighboringField1;
		fieldListDirection[count] = previousDirection;
		count++;

		struct Field *neighboringField2 = getNeighboringFieldInDirection(map, (field)->x, (field)->y, DOWN);
		fieldList[count] = neighboringField2;
		fieldListDirection[count] = DOWN;
		count++;

		struct Field *neighboringField3 = getNeighboringFieldInDirection(map, (field)->x, (field)->y, RIGHT);
		fieldList[count] = neighboringField3;
		fieldListDirection[count] = RIGHT;
		count++;
	}
		for (int i = 0; i < count; i++)
		{
			if(fieldList[i]->population_type == PREY && popuType == PREDATOR)
			{
				return 1;
			}
			//else if(fieldList[i]->containsPlant && popuType == PREY)
			//{
			//	return 1;
			//}
			else if(layer == 0)
			{
				if (findFood(map, fieldList[i], fieldListDirection[i], 1, popuType) == 1)
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
		neighboringField->population_type = field->population_type;
		neighboringField->energy = MAX_ENERGY;
		return;
	}
}

/**
 * gibt ein zufälliges benachbartes Feld zurück
 *
 */
struct Field* getRandomNeighboringField(struct Map *map, struct Field *field)
{
	enum Direction direction = rand() % NUMBER_OF_DIRECTIONS;
	struct Field *neighboringField = getNeighboringFieldInDirection(map, field->x, field->y, direction);

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
	for (int i = 0; i < NUMBER_OF_DIRECTIONS; i++)
	{
		struct Field *neighboringField = getNeighboringFieldInDirection(map, field->x, field->y, direction);
		if (neighboringField->population_type == EMPTY)
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
struct Field* getNeighboringFieldInDirection(struct Map *map, int x, int y, enum Direction direction)
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

