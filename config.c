#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "config.h"
#include "map.h"
#include "parallel.h"

const int PRINTING_ENABLED = 0;

const char SAVE_PATH[] = "/tmp/pred/"; // Pfad zu Ordner für Bitmap-Erzeugung mit abschließendem /
const char BITMAP_FILENAME[] = "%d.bmp"; // Dateiname für Bitmap (muss ein %d für die Nummer des Schrittes enthalten)

const int MAP_WIDTH = 1000; // Breite der Karte
const int MAP_HEIGHT = 1000; // Höhe der Karte
const int SCALE_FACTOR = 1; // Skalierungsfaktor für die Bildausgabe
const int MAX_SIMULATION_STEPS = 20; // Anzahl der Simulationsschritte -1 für unendlich

const float MAP_FILL_RATE = 0.3;
const float PREDATOR_RATE = 0.2; // an der MAP_FILL_RATE
const float PREY_RATE = 0.8; // an der MAP_FILL_RATE
const float PLANT_RATE = 0.2; // an der Gesamtmap

//const float MAP_FILL_RATE = 1;
//const float PREDATOR_RATE = 0.5; // an der MAP_FILL_RATE
//const float PREY_RATE = 0.5; // an der MAP_FILL_RATE
//const float PLANT_RATE = 0.5; // an der Gesamtmap

const int ELDERLY_AGE[NUMBER_OF_POPULATION_TYPES] = {
	0, // EMPTY
	30, // PREY
	50 // PREDATOR
};

const int MAX_ENERGY = 5;

const float DYING_RATE[NUMBER_OF_POPULATION_TYPES] = {
	0, // EMPTY
	0.005, // PREY
	0.02 // PREDATOR
};

const float BIRTH_RATE[NUMBER_OF_POPULATION_TYPES] = {
	0, // EMPTY
	0.2, // PREY
	0.15 // PREDATOR
};

/**
 * get a random number between (and including) low and high
 */
int random_int(const int low, const int high)
{
	return low + (double)rand () * (high - low + 1) / RAND_MAX;
}

/**
 * get a random number between (and including) low and high
 */
void output(const char* format, ...)
{
	if(1) // disable for debugging
		return;

	va_list args;
	va_start(args, format);

	vprintf(format, args);

	va_end(args);
}
