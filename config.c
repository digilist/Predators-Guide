#include "config.h"
#include "map.h"

const int PRINTING_DISABLED = 1;

const char BITMAP_PATH[] = "/tmp/pred/"; // Pfad zu Ordner für Bitmap-Erzeugung mit abschließendem /
const char BITMAP_FILENAME[] = "%d.bmp"; // Dateiname für Bitmap (muss ein %d für die Nummer des Schrittes enthalten)

const int MAP_WIDTH = 100; // Breite der Karte
const int MAP_HEIGHT = 100; // Höhe der Karte
const int SCALE_FACTOR = 1; // Skalierungsfaktor für die Bildausgabe
const int SIMULATION_STEPS = 10; // Anzahl der Simulationsschritte -1 für unendlich

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

