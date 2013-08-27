#ifndef RUNTIME_CONFIGURATION_H_INCLUDED
#define RUNTIME_CONFIGURATION_H_INCLUDED

struct RuntimeConfigurationList {
	struct RuntimeConfiguration *config;
	struct RuntimeConfigurationList *next;
};

struct RuntimeConfiguration {
	int		mapWidth,
			mapHeight;
	float	mapFillRate,
			preyRate,
			predatorRate;
};

struct RuntimeConfigurationList* generateRuntimes();

#endif
