#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "helpers.h"
#include "simulation.h"
#include "map.h"
#include "fields.h"
#include "result.h"
#include "parallel.h"

struct Coordinates
{
	int x;
	int y;
};

struct StepResult* calculate_step_result(int step);

void get_movement_order(struct Field **movements, int fields);
void shuffle(void **elements, int count);

struct Field* check_for_food(struct Field *field);
struct Field* move_animal(struct Field *field);

void create_child(struct Field *field);
int should_get_child(int population_type);
int should_die(struct Field *field);

struct Field* get_random_empty_neighboring_field(struct Field *field);
struct Field* get_neighboring_field_in_direction(int x, int y, enum Direction direction);

// operations in this simulation step
unsigned long _operations = 0;

/**
 * simulate a single step
 *
 */
void simulation_step(int step)
{
	_operations =  0;

	struct Segment *segment = get_segment();

	irecv_field();

	int fields = segment->width * segment->height;
	struct Field **movements = malloc(sizeof(struct Field *) * fields);
	get_movement_order(movements, fields);

	for (int i = 0; i < fields; i++)
	{
		_operations++;

		struct Field *field = movements[i];

		if(is_near_border(field))
			irecv_field();

		if (field->population_type != EMPTY)
		{
			check_for_food(field);
			// suche nach Beute
		}
	}

	get_movement_order(movements, fields);
	for (int i = 0; i < fields; i++)
	{
		_operations++;

		struct Field *field = movements[i];

		if(is_near_border(field))
			irecv_field();

		if (field->population_type != EMPTY)
		{
			// if the animal becomes to old or starves, it will die
			if (should_die(field))
			{
				reset_field(field);
			}
			else
			{
				// has the animal on this field been moved before? (e.g. because it has found food)
				if(field->last_step < step)
				{
					struct Field *moved = move_animal(field);
					if(moved != 0)
					{
						field->last_step++;
						if(is_field_in_segment(moved))
						{
							field = moved;
						}
						else
						{
							field = 0; // animal has moved, but is now out of our segment
						}
					}
				}

				if(field != 0)
				{
					// check, whether the animal is old enough and should get a child
					if (should_get_child(field->population_type))
					{
						create_child(field);
					}

					field->energy -= 2;
					field->age++;
				}
			}
		}
		else if(field->population_type == EMPTY)
		{
			if(random_int(1, 100) <= (PLANT_RATE * 100))
			{
				field->contains_plant = 1;
			}
		}
	}

	irecv_field();

	free(movements);
}

/**
 * get a result (amount of animals / plants) of the current simulation step
 *
 */
struct StepResult* calculate_step_result(int step)
{
	// statistics
	struct StepResult *resultset = malloc(sizeof(struct StepResult));
	resultset->amount_prey = 0;
	resultset->amount_predators = 0;
	resultset->amount_plants = 0;
	resultset->current_step = step;
	resultset->operations = _operations;
	resultset->next = 0;

	struct Segment *segment = get_segment();

	for (int x = segment->x1; x <= segment->x2; x++)
	{
		for (int y = segment->y1; y <= segment->y2; y++)
		{
			struct Field *field = get_field(x, y);

			if (field->population_type == PREY)
			{
				resultset->amount_prey++;
			}
			else if (field->population_type == PREDATOR)
			{
				resultset->amount_predators++;
			}

			if (field->contains_plant)
			{
				resultset->amount_plants++;
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
	return field->energy <= 0 ||
			field->age > ELDERLY_AGE[field->population_type] ||
			(DYING_RATE[field->population_type] > 0 && random_int(1, 100) <= dying_rate);
}

/**
 * calculate, if the population_type should create a new child
 *
 */
int should_get_child(int population_type)
{
	int birth_rate = BIRTH_RATE[population_type] * 100;
	return BIRTH_RATE[population_type] > 0 && random_int(1, 100) <= birth_rate;
}

/**
 * get a shuffled list of all fields, to create a preferably random behaviour
 *
 */
void get_movement_order(struct Field **movements, int fields)
{
	struct Segment *segment = get_segment();

	int i = 0;
	for (int x = segment->x1; x <= segment->x2; x++)
	{
		for (int y = segment->y1; y <= segment->y2; y++)
		{
			movements[i] = get_field(x, y);
			i++;
		}
	}

	shuffle((void **) movements, fields);
}

int fight(struct Field *predator, struct Field *prey);

/**
 * let a predator fight with a prey
 * returns 1, if the predator wins and eats the prey
 *
 * always one creature will die!
 */
int fight(struct Field *predator, struct Field *prey)
{
	int predator_strengh = predator->energy + MAX_ENERGY;
	int prey_strengh = prey->energy;

	int predator_energy = predator->energy * 3; // predators have a bonus
	int prey_energy = prey->energy;

	int rounds = 0;
	while(predator_energy > 0 && prey_energy > 0)
	{
		int x = random_int(0, predator_strengh);
		int y = random_int(0, prey_strengh);

		if(x > y) // predator wins
		{
			prey_energy -= 3;
		}
		else if(x < y) // prey wins
		{
			predator_energy -= 2;
		}
		else // remis
		{
			predator_energy--;
			prey_energy--;
		}

		rounds++;
	}

	if(prey_energy > 0)
	{
		prey->energy = prey_energy;
		return 0;
	}

	return 1;
}

/**
 * search for prey on any neighboring field and eat it, if there is one
 * when the animal eats the prey, it will move forward to the neighboring field
 * and returns it new position
 *
 * otherwise, this method will return 0
 */
struct Field* check_for_food(struct Field *field)
{
	for (int i = 0; i < NUMBER_OF_DIRECTIONS; i++) // alle umliegenden Felder prüfen
	{
		struct Field *neighboring_field = get_neighboring_field_in_direction(field->x, field->y, i);

		if(field->population_type == PREDATOR)
		{
			if (neighboring_field->population_type == PREY) // Predator eats prey
			{
				// fight!
				if(fight(field, neighboring_field))
				{
					field->energy = MAX_ENERGY;
					field->last_step++;
					move_field_to(field, neighboring_field);
				}
				else
				{
					reset_field(field);
					return 0;
				}
			}
		}
		else if(field->population_type == PREY)
		{
			if (neighboring_field->contains_plant) // Prey eats Plants
			{
				field->last_step++;
				field->energy = MAX_ENERGY;
				field->contains_plant = 0;
				move_field_to(field, neighboring_field);

				return neighboring_field;
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
struct Field* move_animal(struct Field *field)
{
	struct Field *neighboring_field = get_random_empty_neighboring_field(field);

	if (neighboring_field)
	{
		move_field_to(field, neighboring_field);
		return neighboring_field;
	}

	return 0;
}

/**
 * create a child on any empty neighboring field
 *
 */
void create_child(struct Field *field)
{
	struct Field *neighboring_field = get_random_empty_neighboring_field(field);
	if (neighboring_field)
	{
		neighboring_field->population_type = field->population_type;
		neighboring_field->energy = MAX_ENERGY;

		send_field(neighboring_field);
	}
}

/**
 * get a random neighboring field
 *
 */
struct Field* get_random_neighboring_field(struct Field *field)
{
	enum Direction direction = rand() % NUMBER_OF_DIRECTIONS;
	struct Field *neighboring_field = get_neighboring_field_in_direction(field->x, field->y, direction);

	return neighboring_field;
}

/**
 * get a random and empty neighboring field
 *
 */
struct Field* get_random_empty_neighboring_field(struct Field *field)
{
	enum Direction direction = rand() % NUMBER_OF_DIRECTIONS;
	for (int i = 0; i < NUMBER_OF_DIRECTIONS; i++)
	{
		struct Field *neighboring_field = get_neighboring_field_in_direction(field->x, field->y, direction);
		if (neighboring_field->population_type == EMPTY)
		{
			return neighboring_field;
		}

		direction = (direction + 1) % NUMBER_OF_DIRECTIONS;
	}

	return 0;
}

/**
 * gibt das benachbarte Feld in der gegebenen Richtung zurück
 */
struct Field* get_neighboring_field_in_direction(int x, int y, enum Direction direction)
{
	struct Map *map = get_map();

	if (direction == UP || direction == UP_LEFT || direction == UP_RIGHT)
	{
		y -= 1;
	}
	if (direction == DOWN || direction == DOWN_LEFT || direction == DOWN_RIGHT)
	{
		y += 1;
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

	return get_field(x, y);
}

