#include "config.h"
#include "map.h"

const char BITMAP_PATH[] = "/tmp/pred/"; // Pfad zu Ordner für Bitmap-Erzeugung mit abschließendem /
const char BITMAP_FILENAME[] = "%d.bmp"; // Dateiname für Bitmap (muss ein %d für die Nummer des Schrittes enthalten)

const int MAP_WIDTH = 200; // Breite der Karte
const int MAP_HEIGHT = 200; // Höhe der Karte
const int SCALE_FACTOR = 3; // Skalierungsfaktor für die Bildausgabe
const int SIMULATION_STEPS = 1000; // Anzahl der Simulationsschritte -1 für unendlich

const int MATURITY_AGE[NUMBER_OF_POPULATION_TYPES] = {
	0,
	0,
	4, // HERBIVORE
	6, // CARNIVORE
};

const int ELDERLY_AGE[NUMBER_OF_POPULATION_TYPES] = {
	0,
	10,
	18, // HERBIVORE
	14, // CARNIVORE
};

const int MAX_STARVE_TIME[NUMBER_OF_POPULATION_TYPES] = {
	0,
	0,
	4, // HERBIVORE
	2, // CARNIVORE
};
