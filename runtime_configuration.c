#include <stdlib.h>
#include <stdio.h>

#include "runtime_configuration.h"

/**
 * Generiert die verschiedenen möglichen Runtime-Konfigurationen, die man vergleichen möchte
 */
struct RuntimeConfigurationList* generateRuntimes()
{
	struct RuntimeConfigurationList *list = 0;
	struct RuntimeConfigurationList *last;

	// testcase
//	list = last = malloc(sizeof(struct RuntimeConfigurationList));
//	list->next = 0;
//	list->config = malloc(sizeof(struct RuntimeConfiguration));
//
//	list->config->mapWidth= 100;
//	list->config->mapHeight = 100;
//	list->config->mapFillRate = 0.5;
//	list->config->predatorRate = 0.8;
//	list->config->preyRate = 0.2;
//
//	return list;

	for (int size = 100; size <= 100; size += 100)
	{
		for (float fillRate = 0.1; fillRate <= 0.5; fillRate += 0.1)
		{
			for (float predRate = 0.1; predRate <= 0.9; predRate += 0.1)
			{
				struct RuntimeConfiguration *rc = malloc(sizeof(struct RuntimeConfiguration));
				rc->mapWidth = rc->mapHeight = size;
				rc->mapFillRate = fillRate;
				rc->predatorRate = predRate;
				rc->preyRate = 1 - predRate;

				if(!list)
				{
					list = last = malloc(sizeof(struct RuntimeConfigurationList));
				}
				else
				{
					last->next = malloc(sizeof(struct RuntimeConfigurationList));
					last = last->next;
				}

				last->next = 0;

				last->config = rc;
			}
		}
	}

	return list;
}
