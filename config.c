#include "config.h"
#include "map.h"

const char BITMAP_PATH[] = "/tmp/pred/"; // Pfad zu Ordner für Bitmap-Erzeugung mit abschließendem /
const char BITMAP_FILENAME[] = "%d.bmp"; // Dateiname für Bitmap (muss ein %d für die Nummer des Schrittes enthalten)

const int MAP_WIDTH = 1366; // Breite der Karte
const int MAP_HEIGHT = 768; // Höhe der Karte
const int SCALE_FACTOR = 1; // Skalierungsfaktor für die Bildausgabe
const int SIMULATION_STEPS = 500; // Anzahl der Simulationsschritte -1 für unendlich

const int MATURITY_AGE[NUMBER_OF_POPULATION_TYPES] = {
	0, // EMPTY
	0, // PLANT
	4, // HERBIVORE
	6 // CARNIVORE
};

const int ELDERLY_AGE[NUMBER_OF_POPULATION_TYPES] = {
	0, // EMPTY
	10, // PLANT
	18, // HERBIVORE
	14 // CARNIVORE
};

const int MAX_STARVE_TIME[NUMBER_OF_POPULATION_TYPES] = {
	0, // EMPTY
	0, // PLANT
	4, // HERBIVORE
	2 // CARNIVORE
};
