#include "config.h"
#include "map.h"

const int PRINTING_DISABLED = 1;

const char BITMAP_PATH[] = "/tmp/pred/"; // Pfad zu Ordner für Bitmap-Erzeugung mit abschließendem /
const char BITMAP_FILENAME[] = "%d.bmp"; // Dateiname für Bitmap (muss ein %d für die Nummer des Schrittes enthalten)

const int MAP_WIDTH = 1000; // Breite der Karte
const int MAP_HEIGHT = 1000; // Höhe der Karte
const int SCALE_FACTOR = 1; // Skalierungsfaktor für die Bildausgabe
const int SIMULATION_STEPS = 500; // Anzahl der Simulationsschritte -1 für unendlich

const int ELDERLY_AGE[NUMBER_OF_POPULATION_TYPES] = {
	0, // EMPTY
	18, // HERBIVORE
	14 // CARNIVORE
};

const int MAX_STARVE_TIME[NUMBER_OF_POPULATION_TYPES] = {
	0, // EMPTY
	10000000, // HERBIVORE
	5 // CARNIVORE
};

const float DYING_RATE[NUMBER_OF_POPULATION_TYPES] = {
	0, // EMPTY
	0.005, // HERBIVORE
	0.02 // CARNIVORE
};

const float BIRTH_RATE[NUMBER_OF_POPULATION_TYPES] = {
	0, // EMPTY
	0.2, // HERBIVORE
	0.12 // CARNIVORE
};

