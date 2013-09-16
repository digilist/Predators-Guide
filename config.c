#include <stdlib.h>

#include "config.h"
#include "map.h"

const int PRINTING_ENABLED = 1;

const char SAVE_PATH[] = "/tmp/pred/"; // Pfad zu Ordner für Bitmap-Erzeugung mit abschließendem /
const char BITMAP_FILENAME[] = "%d.bmp"; // Dateiname für Bitmap (muss ein %d für die Nummer des Schrittes enthalten)

const int MAP_WIDTH = 100; // Breite der Karte
const int MAP_HEIGHT = 100; // Höhe der Karte
const int SCALE_FACTOR = 10; // Skalierungsfaktor für die Bildausgabe
const int MAX_SIMULATION_STEPS = 500; // Anzahl der Simulationsschritte -1 für unendlich

const float MAP_FILL_RATE = 0.3;
const float PREDATOR_RATE = 0.3;
const float PREY_RATE = 0.7;

const int ELDERLY_AGE[NUMBER_OF_POPULATION_TYPES] = {
	0, // EMPTY
	30, // PREY
	50 // PREDATOR
};

const int PREDATOR_MAX_STARVE_TIME = 5;

const float DYING_RATE[NUMBER_OF_POPULATION_TYPES] = {
	0, // EMPTY
	0.005, // PREY
	0.02 // PREDATOR
};

const float BIRTH_RATE[NUMBER_OF_POPULATION_TYPES] = {
	0, // EMPTY
	0.2, // PREY
	0.08 // PREDATOR
};

int randomInt(int low, int high)
{
	return low + (double)rand () * (high - low + 1) / RAND_MAX;
}
